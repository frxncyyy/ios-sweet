#!/usr/bin/env python3
"""Add a fixed audio bundle and board configuration to a verified existing Nord2 ADT.
Never reconstruct the baseline: keep its private Wi-Fi calibration and all other
properties intact. The firmware blob and the output stay outside Git.
"""
import argparse,hashlib,importlib.util,pathlib,struct
import adt
spec=importlib.util.spec_from_file_location('audio_package',pathlib.Path(__file__).with_name('nord2-audio-package.py'))
pkg=importlib.util.module_from_spec(spec);spec.loader.exec_module(pkg)
BOARD='635300fbc96f258ff9a35f82435f2e9d3ff6f1370c156067b40146e011038692'
def add(baseline,package,board):
 root,end=adt.parse(baseline)
 if end!=len(baseline):raise ValueError('trailing ADT bytes')
 chosen=[c for c in root.children if c.name=='chosen']
 if len(chosen)!=1:raise ValueError('one chosen node required')
 names=[n for n,f,b in chosen[0].props]
 if 'nord2-audio-package' in names or 'nord2-audio-board' in names:raise ValueError('audio payload already present')
 pkg.decode(package)
 if len(board)!=4163 or hashlib.sha256(board).hexdigest()!=BOARD:raise ValueError('wrong Nord2 amplifier configuration')
 chosen[0].props.extend([('nord2-audio-package',0,package),('nord2-audio-board',0,board)])
 result=adt.serialize(root)
 # Removing exactly the two additions reproduces every byte of the baseline.
 verify,_=adt.parse(result);c=[n for n in verify.children if n.name=='chosen'][0]
 c.props=[p for p in c.props if p[0]not in('nord2-audio-package','nord2-audio-board')]
 if adt.serialize(verify)!=baseline:raise ValueError('non-audio baseline changed')
 return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for n in ('baseline','package','board','output'):p.add_argument(n,type=pathlib.Path)
 a=p.parse_args();blob=add(a.baseline.read_bytes(),a.package.read_bytes(),a.board.read_bytes())
 with a.output.open('xb')as f:f.write(blob)
 print(f'{len(blob)}bytes SHA256{hashlib.sha256(blob).hexdigest()}')
