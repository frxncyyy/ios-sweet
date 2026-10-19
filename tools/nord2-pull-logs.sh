#!/bin/bash
# Collect everything that can survive from the previous boot of the Nord 2.
#
#   tools/nord2-pull-logs.sh [LABEL]
#
# Run it as soon as Android is back after a test. It waits for adb and
# sys.boot_completed, then saves into backup/nord2/logs/<time>[-LABEL]/:
#   bootreason.txt   androidboot.bootreason + full cmdline (why we rebooted)
#   pstore/*         every file in /sys/fs/pstore (pmsg-ramoops-0 holds the
#                    previous boot's pmsg ring, console-ramoops-0 its console)
#   ramoops.txt      ramoops parameters and the kernel's persistent_ram lines
#   dmesg.txt        current kernel log
#   SHA256SUMS
# pstore only has data if the reset kept DRAM: a PMIC cold reboot (long power
# press, battery pull) clears it, and the script says so explicitly.
set -euo pipefail
ADB=${ADB:-/home/frxncy/Android/Sdk/platform-tools/adb}
cd "$(dirname "$0")/.."
out=backup/nord2/logs/$(date +%Y%m%d-%H%M%S)${1:+-$1}
mkdir -p "$out/pstore"

root() { "$ADB" exec-out "su -c $(printf '%q' "$1")"; }

echo "waiting for adb..."
"$ADB" wait-for-device
for _ in $(seq 120); do
	[ "$("$ADB" shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" = 1 ] && break
	sleep 1
done

root 'cat /proc/cmdline' | tr ' ' '\n' > "$out/cmdline.txt"
reason=$(grep '^androidboot.bootreason=' "$out/cmdline.txt" | cut -d= -f2- || true)
{ echo "bootreason=${reason:-unknown}"; echo; cat "$out/cmdline.txt"; } > "$out/bootreason.txt"

for f in $(root 'ls /sys/fs/pstore' | tr -d '\r'); do
	root "cat /sys/fs/pstore/$f" > "$out/pstore/$f"
done
{
	root 'grep . /sys/module/ramoops/parameters/*'
	echo
	root 'dmesg' | grep -i -E 'persistent_ram|ramoops|pstore' || true
} > "$out/ramoops.txt"
root 'dmesg' > "$out/dmesg.txt"
(cd "$out" && find . -type f ! -name SHA256SUMS -print0 | sort -z | xargs -0 sha256sum > SHA256SUMS)

echo "saved in $out"
echo "bootreason: ${reason:-unknown}"
n=$(find "$out/pstore" -type f -size +0 | wc -l)
if [ "$n" = 0 ]; then
	echo "pstore EMPTY: nothing survived from the previous boot."
	case "$reason" in
	*cold*) echo "(cold reset: DRAM was not preserved, so this is expected)";;
	esac
else
	ls -l "$out/pstore"
	[ -s "$out/pstore/pmsg-ramoops-0" ] && { echo "--- pmsg-ramoops-0 (tail) ---"; tail -c 2000 "$out/pstore/pmsg-ramoops-0" | strings; }
fi
