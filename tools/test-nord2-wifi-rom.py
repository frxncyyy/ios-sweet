#!/usr/bin/env python3
"""Test the actual ROM policy, placement and power state machines off-device.

The saved device DT, vendor image and authenticated firmware remain private.
No SMC, MMIO or device command is performed by these host fixtures.
"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / 'shim/nord2-xnu'
FIX = ROOT / 'tools/fixtures'


def main():
    firmware = ROOT / 'research/nord2/wifi/firmware'
    cases = [
        ('contract', ['wifi_rom_contract.c', 'wifi_rom_fdt.c'], [],
         [ROOT / 'dtb/nord2/live.dtb']),
        ('place', ['wifi_rom_place.c', 'wifi_rom_contract.c', 'wifi_rom_plan.c'],
         ['-DNRB_HOST_TEST', '-DNWF_ROM_HOST_TEST', '-lcrypto'],
         [firmware / 'soc3_0_ram_wmmcu_1a_1_hdr.bin',
          firmware / 'soc3_0_ram_wifi_1a_1_hdr.bin']),
        ('native', ['wifi_rom_xnu.c', 'wifi_rom_place.c', 'wifi_rom_contract.c',
                    'wifi_rom_plan.c', 'wifi_firmware.c'],
         ['-DNRB_HOST_TEST', '-DNRB_NATIVE_HOST_TEST', '-DNWF_ROM_HOST_TEST',
          '-DNWF_HOST_TEST', '-lcrypto'],
         [ROOT / 'research/nord2/wifi/vendor-read/vendor.img']),
        ('powered', ['wifi_rom_powered.c', 'wifi_rom_powered_adapter.c'], [], []),
        ('bind', ['wifi_rom_powered.c', 'wifi_rom_powered_adapter.c'], [], []),
    ]
    with tempfile.TemporaryDirectory() as directory:
        for name, sources, extra, args in cases:
            binary = Path(directory) / name
            subprocess.run([
                'cc', '-std=c11', '-O1', '-g', '-Wall', '-Wextra', '-Werror',
                '-Wno-deprecated-declarations', '-fsanitize=address,undefined',
                '-fno-omit-frame-pointer', '-I' + str(SRC), '-I' + str(FIX),
                str(FIX / ('nord2-wifi-rom-' + name + '.c')),
                *map(lambda s: str(SRC / s), sources), *extra,
                '-o', str(binary)], check=True)
            subprocess.run([str(binary), *map(str, args)], check=True)
            print('PASS actual canonical ROM ' + name, flush=True)


if __name__ == '__main__':
    main()
