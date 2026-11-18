import sys, tempfile, os, json
KEEP = '/tmp/kb72'
os.makedirs(KEEP, exist_ok=True)
class KeepDir:
    def __init__(self, prefix=None, dir=None): self.name = KEEP
    def __enter__(self): return KEEP
    def __exit__(self, *a): return False
tempfile.TemporaryDirectory = KeepDir
sys.path.insert(0, '/home/kai-luni/hidrive/linux_sync/operations/future_cop/fcq')
import fcq
from pathlib import Path
W = Path('/home/kai-luni/git/future-cop-lapd-ps1/build/us/fcq/work/func_80072F7C-20261104-183222-ed9eaa46')
r = fcq.measure('func_80072F7C', W/'candidate.c', fcq.read_json(W/'profile.json', {}))
print('exact', r.get('exact'), 'words', r.get('equal_words'), '/', r.get('total_words'), 'emitted', r.get('emitted_bytes'), 'mm', len(r.get('mismatches', [])))
act = r.get('actual')
if act is not None:
    open('/tmp/kb72_actual.bin','wb').write(bytes(act))
    print('wrote actual', len(act))
