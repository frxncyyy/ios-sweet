#!/usr/bin/env python3
"""Exercise the opt-in common-register transaction and its native adapter.

The selected Android checkout is an independent oracle for register order and
masks. No device access is performed. Tests inject partial writes, lost access,
and restoration failures, including the path that must prohibit Linux handoff.
"""
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CANON = ROOT / "shim/nord2-xnu"
ANDROID = ROOT.parent / "nord2-current-kernel"
PLATFORM = ANDROID / "drivers/misc/mediatek/connectivity/conninfra/platform/mt6893"
ACTIVE = ROOT / "research/ci-39158169212/buildtree/xnu"

source = (PLATFORM / "mt6893_pos.c").read_text()
source = source.split("int connsys_spi_master_cfg_mt6893(")[1].split("\n//#ifndef")[0]
source = re.sub(r"/\*.*?\*/", "", source, flags=re.S)
source = re.sub(r"if \(bt_only\) \{.*?\} else \{(.*?)\}", r"\1", source, flags=re.S)
defines = dict(re.findall(
    r"#define\s+(CONN_WT_SLP_CTL_REG_WB_\w+)\s+\(?\s*(0x[0-9a-fA-F]+)",
    (PLATFORM / "include/mt6893_consys_reg_offset.h").read_text()))
expected = []
for masked, name, value, mask in re.findall(
    r"CONSYS_REG_WRITE(_MASK)?\(\s*CON_REG_WT_SPL_CTL_ADDR\s*\+\s*(\w+),"
    r"\s*(0x[0-9a-fA-F]+)(?:,\s*(0x[0-9a-fA-F]+))?\s*\)", source):
    expected.append((0x18005000 + int(defines[name], 16),
                     int(mask, 16) if masked else 0xffffffff, int(value, 16)))
actual = [tuple(int(x, 16) for x in row) for row in re.findall(
    r"\{0x([0-9a-f]+)u,0x([0-9a-f]+)u,0x([0-9a-f]+)u\}",
    (CANON / "wifi_common.c").read_text())]
assert len(expected) == 36 and actual[4:] == expected
assert actual[:4] == [(0x18000038, 0xffffffff, 0),
                      (0x1800003c, 0xffffffff, 255),
                      (0x18000050, 0xffffffff, 0),
                      (0x18000054, 0xffffffff, 1)]

touch = (ACTIVE / "bsd/dev/sweet_touch.c").read_text()
assert touch.index("ncc_fresh_owned = true;") < touch.index("nw.saved = true;")
assert touch.index("ncc_fresh_owned = true;") > touch.index("power probe refused PMIC slot=")
assert touch.index("ncc_fresh_owned = true;") > touch.index("(nw.pwr & 0x1f) != 0x12")
assert "!ni_pmic_read(0x1c20, &mon13) || mon13" in touch
assert "!ni_pmic_read(0x1c32, &mon18) || mon18" in touch
assert "cfg_id = ni_rd(conn, 4);\n\tncc_power_window(host, conn, id);" in touch
assert "nw_restore(void)\n{\n\tif (ncc_latched_dirty) { return false; }" in touch
assert "nord2_input_prepare_return(void)\n{\n\tif (ncc_latched_dirty) { return -1; }" in touch
assert touch.index("ncc_power_window(host, conn, id);") < touch.index("\n\tsuccess = true;\nout:")
for include in ("wifi_common.c", "wifi_cold_snapshot.c", "wifi_common_adapter.inc"):
    assert '#include "' + include + '"' in touch

with tempfile.TemporaryDirectory() as directory:
    temp = Path(directory)
    for fixture in ("engine", "adapter"):
        exe = temp / fixture
        subprocess.run([
            "clang", "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
            "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
            "-I" + str(CANON),
            str(ROOT / f"tools/fixtures/nord2-wifi-common-{fixture}.c"),
            str(CANON / "wifi_common.c"), str(CANON / "wifi_cold_snapshot.c"),
            str(CANON / "wifi_common_scope.c"), str(CANON / "wifi_adie_id.c"),
            str(CANON / "wifi_adie_strap.c"),
            "-o", str(exe)], check=True)
        subprocess.run([str(exe)], check=True)
    for helper in ("wifi_common", "wifi_cold_snapshot"):
        subprocess.run([
            "clang", "--target=aarch64-none-elf", "-std=c11", "-O2",
            "-Wall", "-Wextra", "-Werror", "-ffreestanding", "-c",
            str(CANON / (helper + ".c")), "-o", str(temp / (helper + ".o"))], check=True)
print("PASS selected Android table, active hook/dirty guards, native fault tests and ARM64 helpers")
