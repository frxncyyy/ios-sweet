#!/bin/bash
# Collect everything that can survive from the previous boot of the Nord 2.
#
#   tools/nord2-pull-logs.sh [--rescue] [LABEL]
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
# --rescue uses the authenticated root ADB RAM recovery, without su/getprop
# or Android's boot-completion property. It never mounts userdata.
set -euo pipefail
ADB=${ADB:-/home/frxncy/Android/Sdk/platform-tools/adb}
cd "$(dirname "$0")/.."
rescue=0
if [ "${1:-}" = --rescue ]; then
	rescue=1
	shift
fi
if [ $# -gt 1 ] || [[ ${1:-} = *[!a-zA-Z0-9._-]* ]]; then
	echo "usage: $0 [--rescue] [LABEL: letters, digits, dot, dash, underscore]" >&2
	exit 2
fi
out=backup/nord2/logs/$(date +%Y%m%d-%H%M%S)${1:+-$1}
mkdir -p "$out/pstore"

# Shell v2 without a PTY preserves bytes and propagates remote exit status.
# exec-out can report exit0 while a missing-file error appears in its stdout.
root() {
	if [ "$rescue" = 1 ]; then
		"$ADB" shell -T "$1"
	else
		"$ADB" shell -T "su -c $(printf '%q' "$1")"
	fi
}

echo "waiting for adb..."
if [ "$rescue" = 1 ]; then
	timeout 180 "$ADB" wait-for-recovery
	[ "$(root 'id -u')" = 0 ]
	[ "$(root 'cat /nord2-rescue')" = 'ADB RAM recovery; userdata is not mounted' ]
else
	timeout 180 "$ADB" wait-for-device
	ready=0
	for _ in $(seq 120); do
		if [ "$("$ADB" shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" = 1 ]; then
			ready=1
			break
		fi
		sleep 1
	done
	[ "$ready" = 1 ] || { echo "Android boot did not complete" >&2; exit 1; }
fi

root 'cat /proc/cmdline' | tr ' ' '\n' > "$out/cmdline.txt"
root 'cat /proc/mounts' > "$out/mounts.txt"
reason=$(grep '^androidboot.bootreason=' "$out/cmdline.txt" | cut -d= -f2- || true)
{ echo "bootreason=${reason:-unknown}"; echo; cat "$out/cmdline.txt"; } > "$out/bootreason.txt"

for f in $(root 'ls /sys/fs/pstore' | tr -d '\r'); do
	[[ "$f" != *[!a-zA-Z0-9._-]* ]] || { echo "unexpected pstore name" >&2; exit 1; }
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
exit 0
