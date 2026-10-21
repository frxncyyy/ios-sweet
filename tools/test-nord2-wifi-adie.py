#!/usr/bin/env python3
"""Check the scoped MT6893 CHIP_ID read and its failure/cleanup paths offline."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CANON = ROOT / "shim/nord2-xnu"
PLATFORM = ROOT.parent / "nord2-current-kernel/drivers/misc/mediatek/connectivity/conninfra/platform/mt6893"
ACTIVE = ROOT / "research/ci-39158169212/buildtree/xnu/bsd/dev"

source = (PLATFORM / "mt6893_pos.c").read_text()
assert "CONN_RF_SPI_MST_REG_SPI_STA, 5," in source
assert "CONN_RF_SPI_MST_REG_SPI_TOP_ADDR, 0x0000b000, 0x0000a000," in source
offsets = (PLATFORM / "include/mt6893_consys_reg_offset.h").read_text()
for symbol, value in (("SPI_STA", "0x0000"), ("SPI_TOP_ADDR", "0x0050"),
                      ("SPI_TOP_WDAT", "0x0054"), ("SPI_TOP_RDAT", "0x0058")):
    assert any("CONN_RF_SPI_MST_REG_" + symbol in line and value in line
               for line in offsets.splitlines())
assert any("ATOP_CHIP_ID" in line and "0x02c" in line for line in offsets.splitlines())
assert "CONN_SEMA_RFSPI_INDEX = 11" in (PLATFORM / "include/mt6893.h").read_text()
readable = (PLATFORM / "mt6893_consys_reg.c").read_text().split(
    "\nint consys_check_reg_readable(void)\n{")[1].split("int consys_check_reg_readable_for_coredump")[0]
assert "INFRA_TOPAXI_PROTECTEN_STA1_OFFSET, (0x1 << 13)" in readable
assert "CONSYS_SET_BIT(CON_REG_HOST_CSR_ADDR, 0x1)" in readable
assert "CONN_HOST_CSR_TOP_BUS_TIMEOUT_IRQ, 0x1" in readable
adapter = (CANON / "wifi_common_adapter.inc").read_text()
assert "NCC_AP(4,nw.infra,0x228,0x2000,0)" in adapter
touch = (ACTIVE / "sweet_touch.c").read_text()
assert "nw_wait(nw.infra, 0x228, 0x6000, 0)" in touch

helpers = ("wifi_common", "wifi_cold_snapshot", "wifi_common_scope",
           "wifi_adie_id", "wifi_adie_strap")
for name in helpers:
    assert (ACTIVE / (name + ".h")).read_bytes() == (CANON / (name + ".h")).read_bytes()
    expected = '#include <arm64/sweet_early.h>\n#if defined(NORD2)\n' + (
        CANON / (name + ".c")).read_text() + '\n#endif /* NORD2 */\n'
    assert (ACTIVE / (name + ".c")).read_text() == expected
for name in ("wifi_common_adapter.inc", "wifi_adie_adapter.inc"):
    assert (ACTIVE / name).read_bytes() == (CANON / name).read_bytes()
for name in ("wifi_common_scope", "wifi_adie_id", "wifi_adie_strap"):
    assert touch.count('#include "' + name + '.c"') == 1

with tempfile.TemporaryDirectory() as directory:
    temp = Path(directory)
    for fixture in ("id", "native"):
        exe = temp / fixture
        subprocess.run([
            "clang", "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
            "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
            "-I" + str(CANON),
            str(ROOT / f"tools/fixtures/nord2-wifi-adie-{fixture}.c"),
            *[str(CANON / (name + ".c")) for name in helpers],
            "-o", str(exe)], check=True)
        subprocess.run([str(exe)], check=True)
    for name in ("wifi_common_scope", "wifi_adie_id", "wifi_adie_strap"):
        subprocess.run([
            "clang", "--target=aarch64-none-elf", "-std=c11", "-O2",
            "-Wall", "-Wextra", "-Werror", "-ffreestanding", "-c",
            str(CANON / (name + ".c")), "-o", str(temp / (name + ".o"))], check=True)
print("PASS selected source, production parity, conditional ID lifetime/faults and ARM64 helpers")
