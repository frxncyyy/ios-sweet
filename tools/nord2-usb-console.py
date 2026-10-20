#!/usr/bin/env python3
"""Read the native XNU USB console using system libusb.

Protocol 1 captures the first 65524 persistent bytes. Protocol 2 reads the
separate 4 MiB append-only live buffer; both are bounded, not unlimited streams.

Usage: nord2-usb-console.py OUTPUT_DIRECTORY [--wait SECONDS]
Selects product "Nord2 XNU console", never the Android ADB recovery interface.
Only vendor IN requests are sent; there are no disk or memory-write commands.
Linux access: install the adjacent 70-nord2-xnu-console.rules into
/etc/udev/rules.d/ with administrator authentication, then reload udev.
"""
import argparse
import ctypes as C
import ctypes.util
import hashlib
import json
from pathlib import Path
import struct
import time

ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument('output', type=Path)
ap.add_argument('--wait', type=float, default=180)
ap.add_argument('--read-seconds', type=float, default=20)
args = ap.parse_args()
assert 0 < args.wait <= 600
assert 0 < args.read_seconds <= 600
args.output.mkdir(parents=True, exist_ok=True)
lib = C.CDLL(ctypes.util.find_library('usb-1.0') or 'libusb-1.0.so.0')
ptr = C.c_void_p
for name, restype, argtypes in [
    ('init', C.c_int, [C.POINTER(ptr)]),
    ('exit', None, [ptr]),
    ('get_device_list', C.c_ssize_t, [ptr, C.POINTER(C.POINTER(ptr))]),
    ('free_device_list', None, [C.POINTER(ptr), C.c_int]),
    ('get_bus_number', C.c_uint8, [ptr]),
    ('get_device_address', C.c_uint8, [ptr]),
    ('open', C.c_int, [ptr, C.POINTER(ptr)]),
    ('close', None, [ptr]),
    ('control_transfer', C.c_int, [ptr, C.c_uint8, C.c_uint8, C.c_uint16,
                                 C.c_uint16, ptr, C.c_uint16, C.c_uint]),
]:
    f = getattr(lib, 'libusb_' + name)
    f.restype, f.argtypes = restype, argtypes

ctx, handle = ptr(), ptr()
if lib.libusb_init(C.byref(ctx)) != 0:
    raise SystemExit('libusb initialization failed')


def request(code, value, size, index=0):
    buffer = C.create_string_buffer(size)
    # All requests are read-only and log offsets are immutable. An overloaded
    # phone can miss one EP0 deadline while printing or paging in userland;
    # retry the same offset without duplicating or losing captured bytes.
    for attempt in range(4):
        n = lib.libusb_control_transfer(handle, 0xc0, code, value, index,
                                       buffer, size, 1500)
        if n >= 0:
            return buffer.raw[:n]
        event = dict(request=code, value=value, index=index, error=n,
                     attempt=attempt + 1, time=time.time())
        with (args.output / 'transport-events.jsonl').open('a') as events:
            events.write(json.dumps(event) + '\n')
        if n != -7 or attempt == 3:  # Retry TIMEOUT, never a disconnected device.
            raise OSError(f'USB request {code:#x} failed: libusb {n}')
        time.sleep(0.05)


def read_status():
    raw = request(0x52, 0, 28)
    if len(raw) != 28:
        raise OSError('Truncated native console status')
    status = struct.unpack('<7I', raw)
    if status[0] != 0x4253554e or status[1] not in (1, 2):
        raise ValueError('Unexpected native console protocol')
    return status


try:
    deadline = time.monotonic() + args.wait
    selected = None
    print('Waiting for native XNU USB console...', flush=True)
    while time.monotonic() < deadline:
        for node in Path('/sys/bus/usb/devices').iterdir():
            try:
                if (node / 'product').read_text().strip() != 'Nord2 XNU console':
                    continue
                if (node / 'idVendor').read_text().strip() != '18d1' or \
                   (node / 'idProduct').read_text().strip() != 'd001':
                    continue
                selected = int((node / 'busnum').read_text()), int((node / 'devnum').read_text())
                (args.output / 'descriptors.bin').write_bytes((node / 'descriptors').read_bytes())
                break
            except (OSError, ValueError):
                continue
        if selected:
            break
        time.sleep(0.2)
    if not selected:
        raise SystemExit('Native XNU USB console did not enumerate before the deadline')
    devices = C.POINTER(ptr)()
    count = lib.libusb_get_device_list(ctx, C.byref(devices))
    if count < 0:
        raise SystemExit('libusb device listing failed')
    try:
        for i in range(count):
            device = devices[i]
            if (lib.libusb_get_bus_number(device), lib.libusb_get_device_address(device)) == selected:
                access_deadline = time.monotonic() + 5
                while True:
                    result = lib.libusb_open(device, C.byref(handle))
                    if result != -3 or time.monotonic() >= access_deadline:
                        break
                    time.sleep(0.1)  # Allow the dedicated udev access rule to run.
                if result != 0:
                    hint = ('; install/reload tools/70-nord2-xnu-console.rules'
                            if result == -3 else '')
                    raise SystemExit(f'Cannot open native USB console: libusb {result}{hint}')
                break
    finally:
        lib.libusb_free_device_list(devices, 1)
    if not handle:
        raise SystemExit('Enumerated USB console disappeared')
    status = read_status()
    print('XNU USB connected; reading the console log while XNU is running.', flush=True)
    protocol = status[1]
    capacity = 65524
    if protocol == 2:
        info = request(0x53, 0, 8)
        if len(info) != 8:
            raise SystemExit('Truncated live-log metadata')
        capacity, published = struct.unpack('<2I', info)
        if not 65524 <= capacity <= 16 * 1024 * 1024 or published > capacity:
            raise SystemExit('Invalid live-log bounds')
    log = bytearray()
    samples = [status]
    stop_reason = 'deadline'
    transport_error = None
    until = time.monotonic() + args.read_seconds
    while time.monotonic() < until and len(log) < capacity:
        try:
            offset = len(log)
            chunk = request(0x54 if protocol == 2 else 0x51, offset & 0xffff,
                            min(1024, capacity - offset), index=offset >> 16)
            log += chunk
            if offset == 0:
                (args.output / 'xnu-live.log').write_bytes(chunk)
            elif chunk:
                with (args.output / 'xnu-live.log').open('ab') as output:
                    output.write(chunk)
            samples.append(read_status())
        except OSError as error:
            if not log:
                raise
            stop_reason = 'usb_error'
            transport_error = str(error)
            break
        if not chunk:
            time.sleep(0.25)
    if not any(marker in log for marker in (
            b'NORD2 USB: native console for 45 seconds',
            b'NORD2 USB: native console continuous')):
        raise SystemExit('USB answered, but the current native-console boot marker is missing')
    result = dict(native_usb=True, protocol=protocol, log_capacity=capacity, log_bytes=len(log),
                  log_sha256=hashlib.sha256(log).hexdigest(),
                  status_samples=samples, userdata_commands_sent=False,
                  log_capacity_reached=len(log) == capacity,
                  stop_reason='capacity' if len(log) == capacity else stop_reason,
                  transport_error=transport_error)
    (args.output / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print(f'Saved {len(log)} bytes of XNU log over native USB.', flush=True)
    if transport_error:
        print(f'Capture ended early: {transport_error}', flush=True)
finally:
    if handle:
        lib.libusb_close(handle)
    lib.libusb_exit(ctx)
