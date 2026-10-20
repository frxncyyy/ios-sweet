#!/usr/bin/env python3
"""Reject malformed retained-x2 images before packing; no device access."""
import importlib.util
from pathlib import Path

spec = importlib.util.spec_from_file_location('checker', Path(__file__).with_name('nord2-check-xnu.py'))
checker = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checker)
args = 'nord2ufs2live=1 nord2stopat=23 cpus=8 rd=ufsroot0 ufsroot=userdata ufsrw=1 rootrw=1 nord2ufsprofile=1'.split()
markers = [b'NORD2 UFS X2 LIVE POLICY:', b'reason=manual-reset-required',
           b'NORD2 UFS X2 LIVE PROOF:', b'NORD2 UFS X2 LIVE REFUSED:',
           b'link_fields=pre-transition-baseline return_policy=manual-reset-only',
           b'runtime I/O failed; retain DMA/ownership; manual reset required']
kernel = b'\0'.join(markers)
def valid(a, k=kernel):
    return all(ok for _, ok in checker.live_ufs_checks(a, k))
assert valid(args)
assert checker.live_ufs_checks([], b'') == []
assert checker.live_ufs_checks(['nord2ufs2live=0'], b'') == []
for i, token in enumerate(args):
    key = token.split('=', 1)[0]
    for wrong in (key, key + '=bogus', key + '=01', key + '=0'):
        changed = args[:i] + [wrong] + args[i+1:]
        if wrong != 'nord2ufs2live=0':
            assert not valid(changed), changed
    assert not valid(args + [token]), token
    assert not valid(args + [key]), key
    if i:
        assert not valid(args[:i] + args[i+1:]), token
for stop in ('nord2ufs2lane', 'nord2ufs2lane=0', 'nord2ufs2lane=1'):
    assert not valid(args + [stop]), stop
for i in range(len(markers)):
    assert not valid(args, b'\0'.join(markers[:i] + markers[i+1:]))
assert all(ok for _, ok in checker.point23_root_checks(args, kernel))
print('PASS retained-x2 image mode: unique exact RW/8-core args, exclusive mode, manual-reset markers; opt-out unchanged')
