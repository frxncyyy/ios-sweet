#!/usr/bin/env python3
"""Read the bounded native XNU USB diagnostic console using system libusb.

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


def request(code, value, size):
    buffer = C.create_string_buffer(size)
    n = lib.libusb_control_transfer(handle, 0xc0, code, value, 0,
                                   buffer, size, 1500)
    if n < 0:
        raise OSError(f'USB request {code:#x} failed: libusb {n}')
    return buffer.raw[:n]


def read_status():
    raw = request(0x52, 0, 28)
    if len(raw) != 28:
        raise OSError('Truncated native console status')
    status = struct.unpack('<7I', raw)
    if status[:2] != (0x4253554e, 1):
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
    print('XNU USB connected; reading pmsg while XNU is running.', flush=True)
    log = bytearray()
    samples = [status]
    until = time.monotonic() + args.read_seconds
    while time.monotonic() < until and len(log) < 65524:
        try:
            chunk = request(0x51, len(log), min(1024, 65524 - len(log)))
            log += chunk
            (args.output / 'xnu-live.log').write_bytes(log)
            samples.append(read_status())
        except OSError:
            if not log:
                raise
            break
        if not chunk:
            time.sleep(0.25)
    if b'NORD2 USB: native console for 45 seconds' not in log:
        raise SystemExit('USB answered, but the current native-console boot marker is missing')
    result = dict(native_usb=True, log_bytes=len(log),
                  log_sha256=hashlib.sha256(log).hexdigest(),
                  status_samples=samples, userdata_commands_sent=False)
    (args.output / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print(f'Saved {len(log)} bytes of XNU log over native USB.', flush=True)
finally:
    if handle:
        lib.libusb_close(handle)
    lib.libusb_exit(ctx)
