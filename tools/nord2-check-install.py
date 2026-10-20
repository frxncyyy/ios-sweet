#!/usr/bin/env python3
"""Exercise the installer with a local fake ADB; never opens a real device."""
import contextlib
import hashlib
import importlib.util
import io
import json
import os
from pathlib import Path
import sys
import tempfile

spec = importlib.util.spec_from_file_location(
    "installer", Path(__file__).with_name("nord2-install-root.py"))
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)

with tempfile.TemporaryDirectory() as tmp:
    base = Path(tmp)
    data = bytes(range(256)) * 8192  # Binary stdin, including NUL and newlines.
    root = base / "root.img"
    root.write_bytes(data)
    m.ROOT_SIZE = len(data)
    m.ROOT_SHA = hashlib.sha256(data).hexdigest()
    m.BACKUPS = {str(root): m.ROOT_SHA}
    adb = base / "adb"
    adb.write_text("""#!/usr/bin/env python3
import hashlib,json,pathlib,sys
b=pathlib.Path(__file__).parent
s=json.loads((b/'state.json').read_text()); cmd=sys.argv[-1]
with (b/'commands').open('a') as f: f.write(cmd+'\\n')
answers={
 'id -u':'0',
 'cat /nord2-rescue':'ADB RAM recovery; userdata is not mounted',
 'readlink -f /dev/block/by-name/userdata':'/dev/block/sdc57',
 'cat /sys/class/block/sdc57/start':'25231360',
 'cat /sys/class/block/sdc57/size':'224657344',
 'blockdev --getsize64 /dev/block/sdc57':'115024560128',
 'cat /proc/mounts':'rootfs / rootfs rw 0 0',
 'set -o pipefail; command -v dd; command -v sha256sum':'/system/bin/dd\\n/system/bin/sha256sum'
}
if s.get('geometry'): answers['cat /sys/class/block/sdc57/start']='0'
if s.get('mounted'): answers['cat /proc/mounts']='/dev/block/dm-13 /data f2fs rw 0 0'
if s.get('wrong_device'): answers['readlink -f /dev/block/by-name/userdata']='/dev/block/sdc1'
if cmd in answers: print(answers[cmd])
elif cmd=='dd of=/dev/block/by-name/userdata bs=1048576 conv=nocreat,notrunc,fsync status=none':
 data=sys.stdin.buffer.read()
 (b/'target').write_bytes(data[:100] if s.get('write_error') else data)
 if s.get('write_error'): sys.exit(1)
elif cmd=='set -o pipefail; dd if=/dev/block/by-name/userdata bs=1048576 count=2 status=none | sha256sum':
 data=(b/'target').read_bytes()
 if s.get('corrupt'): data=data[:-1]
 print(hashlib.sha256(data).hexdigest()+'  -')
else: raise RuntimeError('unexpected command: '+cmd)
""")
    adb.chmod(0o700)

    def run(name, state, mode=(), failure=False):
        (base / "state.json").write_text(json.dumps(state))
        (base / "target").write_bytes(b"unchanged")
        (base / "commands").write_text("")
        output = base / name
        sys.argv = ["installer", str(root), str(output), "--serial", "mock",
                    "--adb", str(adb), *mode]
        failed = False
        with contextlib.redirect_stdout(io.StringIO()):
            try:
                m.main()
            except (RuntimeError, m.subprocess.CalledProcessError):
                failed = True
        assert failed == failure, name
        return output, (base / "commands").read_text()

    out, commands = run("preflight", {})
    assert (base / "target").read_bytes() == b"unchanged" and "dd of=" not in commands
    assert not json.loads((out / "result.json").read_text())["userdata_write_started"]
    out, commands = run("success", {}, ["--write-userdata"])
    assert (base / "target").read_bytes() == data
    result = json.loads((out / "result.json").read_text())
    assert result["userdata_write_completed"] and result["full_readback_verified"]
    assert commands.count("dd of=") == 1
    for bad in ("geometry", "mounted", "wrong_device"):
        _, commands = run(bad, {bad: True}, ["--write-userdata"], True)
        assert "dd of=" not in commands and (base / "target").read_bytes() == b"unchanged"
    out, _ = run("corrupt", {"corrupt": True}, ["--write-userdata"], True)
    assert not json.loads((out / "result.json").read_text())["full_readback_verified"]
    out, commands = run("write-error", {"write_error": True}, ["--write-userdata"], True)
    result = json.loads((out / "result.json").read_text())
    assert result["userdata_write_started"] and not result["userdata_write_completed"]
    assert not result["full_readback_verified"] and "dd if=" not in commands
    # Image hash refusal precedes even the first ADB query.
    m.ROOT_SHA = "0" * 64
    _, commands = run("bad-image", {}, ["--write-userdata"], True)
    assert not commands
print("PASS: default never writes; exact binary stream/readback; wrong geometry,"
      " mount, device and image refused; partial write and readback mismatch remain failures")
