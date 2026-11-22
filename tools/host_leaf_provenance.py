#!/usr/bin/env python3
"""Verify leaf-suite linked source provenance and debug-object equivalence.

The proof applies only to a fresh build performed by this process. It checks
linker-map text ranges and compares the debug and target object code *and text
relocations*. QEMU exec/nochain verifies each linked function's entry block was
dispatched. Existing suite assertions are attributed by harness markers;
in_asm supplies only additional translation evidence, not retired instructions.
"""
import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import host_coverage as hc
import host_leaf_suite as leaf
import host_test_reach as reach

SECTION = re.compile(r'^\s*\.text\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(.+\.o)\s*$')
CONTINUED = re.compile(r'^\s*(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(.+\.o)\s*$')


def map_text_ranges(text):
    ranges = []
    pending = False
    for line in text.splitlines():
        match = SECTION.match(line)
        if pending and not match:
            match = CONTINUED.match(line)
        pending = line.strip() == '.text'
        if match:
            start, size, path = match.groups()
            ranges.append((int(start, 16), int(size, 16), Path(path).resolve()))
    return ranges


def verify_binding(ranges, target, address, size):
    target = target.resolve()
    if size <= 0:
        raise ValueError('empty symbol')
    matches = [(start, length, obj) for start, length, obj in ranges
               if start <= address and address + size <= start + length]
    if len(matches) != 1 or matches[0][2] != target:
        raise ValueError(f'symbol range not uniquely bound to {target}: {matches}')


def executed_entries(text):
    return {int(match.group(1), 16) for line in text.splitlines()
            if (match := re.match(r'^Trace \d+: \S+ \[[0-9a-fA-F]+/([0-9a-fA-F]+)/', line))}


def command(args, env):
    r = subprocess.run(args, env=env, capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError(f'{args[0]} failed: {r.stderr[-1000:]}')
    return r.stdout


def text_bytes(obj, dest, env):
    command(['mipsel-linux-gnu-objcopy', '-O', 'binary', '-j', '.text', str(obj), str(dest)], env)
    return dest.read_bytes()


def text_relocations(obj, env):
    output = command(['mipsel-linux-gnu-objdump', '-r', '-j', '.text', str(obj)], env)
    return [line.split() for line in output.splitlines()
            if re.match(r'^\s*[0-9a-fA-F]+\s+R_', line)]


def verify_suite(suite):
    marks = leaf.markers(suite)
    if not marks:
        raise RuntimeError(f'no markers: {suite}')
    env = leaf.env()
    with tempfile.TemporaryDirectory(prefix='leaf-proof-', dir=os.environ.get('TMPDIR')) as tmp:
        root = Path(tmp)
        build = root / 'build'
        env.update(DECOMP=str(leaf.REPO), OUT=str(build))
        real_qemu = os.environ.get('QEMU_REAL', str(Path.home() / '.local/share/qemu-user/extracted/usr/bin/qemu-mipsel'))
        executable_wrapper = root / 'qemu-exec'
        executable_wrapper.write_text('#!/bin/sh\nexec "$REAL_QEMU" -d in_asm,exec,nochain -D "$EXEC_TRACE" "$@"\n')
        executable_wrapper.chmod(0o755)
        env.update(QEMU=str(executable_wrapper), REAL_QEMU=real_qemu, EXEC_TRACE=str(root / 'exec.log'))
        # The suite itself remains unmodified: inject a linker wrapper that adds
        # a map to the exact link invocation used for the subsequent QEMU run.
        wrappers = root / 'wrappers'
        wrappers.mkdir()
        linker = Path(hc.BINUTILS / 'mipsel-linux-gnu-ld').resolve()
        wrapper = wrappers / 'mipsel-linux-gnu-ld'
        wrapper.write_text('#!/bin/sh\nexec "$LEAF_REAL_LD" "-Map=$LEAF_MAP" "$@"\n')
        wrapper.chmod(0o755)
        env['LEAF_REAL_LD'] = str(linker)
        env['LEAF_MAP'] = str(build / 'link.map')
        # Suite prepends its binutils directory. Inject the wrapper via a
        # temporary TOOLCHAIN tree with symlinks to the real tools.
        tc = root / 'toolchain'
        tc.mkdir()
        for item in leaf.T.iterdir():
            if item.name != 'mipsel-binutils':
                (tc / item.name).symlink_to(item, target_is_directory=item.is_dir())
        b = tc / 'mipsel-binutils' / 'root' / 'usr' / 'bin'
        b.mkdir(parents=True)
        for item in hc.BINUTILS.iterdir():
            if item.name != 'mipsel-linux-gnu-ld':
                (b / item.name).symlink_to(item)
        (b / 'mipsel-linux-gnu-ld').symlink_to(wrapper)
        lib = tc / 'mipsel-binutils' / 'root' / 'usr' / 'lib'
        lib.symlink_to(leaf.T / 'mipsel-binutils' / 'root' / 'usr' / 'lib', target_is_directory=True)
        env['TOOLCHAIN'] = str(tc)
        run = subprocess.run(['bash', str(leaf.HOST / f'{suite}_test.sh')],
                             env=env, capture_output=True, text=True, timeout=600)
        if run.returncode:
            raise RuntimeError(f'{suite} failed ({run.returncode}): {run.stderr[-2000:]}')
        per = leaf.parse(run.stdout)
        if set(per) != {f'func_{mark["unit"]}' for mark in marks}:
            raise RuntimeError(f'{suite}: assertion output does not match the marked functions')
        ranges = map_text_ranges((build / 'link.map').read_text())
        binary = build / suite
        tracefile = root / 'exec.log'
        if not tracefile.is_file():
            raise RuntimeError('trace missing')
        addresses = {int(m.group(1), 16) for line in tracefile.read_text(errors='ignore').splitlines()
                     if (m := re.match(r'^0x([0-9a-f]+):\s', line))}
        dispatches = executed_entries((root / 'exec.log').read_text(errors='ignore'))
        rows = []
        for mark in marks:
            unit = mark['unit']
            symbol = f'func_{unit}'
            counts = per.get(symbol)
            if not counts or counts['checks'] < 1 or counts['failures']:
                raise RuntimeError(f'{symbol}: no successful assertions')
            obj = build / f'{unit}.o'
            extent = reach.symbol_extent(binary, symbol, env)
            if not extent:
                raise RuntimeError(f'{symbol}: missing ELF extent')
            addr, size = extent
            verify_binding(ranges, obj, addr, size)
            if not any(addr <= a < addr + size for a in addresses):
                raise RuntimeError(f'{symbol}: missing trace of function range')
            if addr not in dispatches:
                raise RuntimeError(f'{symbol}: no executed QEMU entry dispatch')
            debugdir = root / 'debug'
            debugdir.mkdir(exist_ok=True)
            debug, error = hc.compile_unit(unit, debugdir, hc.toolchain_env())
            if error:
                raise RuntimeError(f'{symbol}: {error}')
            equal = (text_bytes(obj, root / 'normal.bin', env) == text_bytes(debug, root / 'debug.bin', env)
                     and text_relocations(obj, env) == text_relocations(debug, env))
            rows.append({'function': symbol, 'checks': counts['checks'], 'link_map_verified': True,
                         'qemu_entry_dispatched': True, 'qemu_range_translated': True,
                         'debug_text_and_relocations_equal': equal})
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('suites', nargs='+')
    parser.add_argument('--json')
    args = parser.parse_args()
    rows = [row for suite in args.suites for row in verify_suite(suite)]
    print(f'{len(rows)} functions: map and assertions verified; '
          f'{sum(r["debug_text_and_relocations_equal"] for r in rows)} debug objects match')
    if args.json:
        Path(args.json).write_text(json.dumps(rows, indent=2) + '\n')
    return 0


if __name__ == '__main__':
    sys.exit(main())
