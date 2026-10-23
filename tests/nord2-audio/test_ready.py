"""Exercise the actual native bridge and worker wait with delayed NI publication."""
from pathlib import Path
import subprocess
import tempfile

R = Path(__file__).resolve().parents[2]
S = R / 'shim/nord2-xnu'
T = Path(__file__).resolve().parent
source = (S / 'audio_device.c').read_text()
def function(signature):
    start = source.index(signature)
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        if source[end] == '{':
            depth += 1
        elif source[end] == '}':
            depth -= 1
        end += 1
    return source[start:end] + '\n'
worker = source[source.index('static void audio_worker('):]
assert worker.index('nal_initialize(') < worker.index('prepare_native_deferred(') < worker.index('nau_hw_init(')
assert worker.count('nau_hw_init(') == 1
with tempfile.TemporaryDirectory() as tmp:
    d = Path(tmp)
    (d / 'ready-worker-helper.inc').write_text(function('static int prepare_native_deferred('))
    exe = d / 'test-ready'
    subprocess.run(['clang', '-std=c11', '-DNORD2', '-O1', '-g',
                    '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
                    '-fno-sanitize-recover=all', '-I' + str(S), '-I' + str(d),
                    str(T / 'test-ready.c'), '-o', str(exe)], check=True)
    for case in ('wait-publish', 'publish-failed', 'publish-unaligned',
                 'map-once', 'no-context', 'worker-ready', 'worker-timeout',
                 'worker-failed', 'worker-map-once'):
        subprocess.run([str(exe), case], check=True)
    (d / 'ready-diagnose.inc').write_text(function('static void diagnose('))
    modules = [S / (name + '.c') for name in
               ('audio_hw', 'audio_afe', 'audio_id', 'audio_i2c8', 'audio_amp', 'audio_board')]
    exe = d / 'test-diagnostics'
    subprocess.run(['clang', '-std=c11', '-DNORD2', '-O1', '-g',
                    '-Wall', '-Wextra', '-Werror', '-Wno-unused-function',
                    '-Wno-misleading-indentation', '-fsanitize=address,undefined',
                    '-fno-sanitize-recover=all', '-I' + str(S), '-I' + str(d),
                    str(T / 'test-diagnostics.c'), *map(str, modules),
                    '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
print('PASS delayed publication, permanent failures, bounded wait and live command servicing; no MMIO before readiness')
