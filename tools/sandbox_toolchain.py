#!/usr/bin/env python3
"""Prepare a separate toolchain for Linux sandboxes that deny i386 syscalls."""

import argparse
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile


def is_i386_elf(path: Path) -> bool:
    if not path.is_file() or not os.access(path, os.X_OK):
        return False
    with path.open('rb') as executable:
        header = executable.read(20)
    return header[:6] == b'\x7fELF\x01\x01' and header[18:20] == b'\x03\x00'


def prepare(source: Path, target: Path, qemu: Path) -> None:
    if target.exists() or target.is_symlink() or target.is_relative_to(source):
        raise ValueError(f'destination must be new and outside the source toolchain: {target}')
    if not (source / 'gcc-2.7.2-cdk/gcc').is_file():
        raise ValueError(f'not a bootstrapped PS1 toolchain: {source}')
    subprocess.run([qemu, '--version'], check=True, stdout=subprocess.DEVNULL)
    # Publish only a complete overlay, leaving the original toolchain untouched.
    with tempfile.TemporaryDirectory(prefix='.toolchain-', dir=target.parent) as temp:
        staged = Path(temp) / 'overlay'
        staged.mkdir()
        for entry in source.iterdir():
            out = staged / entry.name
            if not (entry.is_dir() and entry.name.startswith('gcc-')):
                out.symlink_to(entry, target_is_directory=entry.is_dir())
                continue
            out.mkdir()
            for tool in entry.iterdir():
                dest = out / tool.name
                if not is_i386_elf(tool):
                    dest.symlink_to(tool, target_is_directory=tool.is_dir())
                    continue
                dest.with_suffix(dest.suffix + '.i386').symlink_to(tool)
                dest.write_text(f'#!/bin/sh\nexec {shlex.quote(str(qemu))} "$0.i386" "$@"\n')
                dest.chmod(0o755)
        staged.rename(target)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path, help='existing, native .toolchain directory')
    parser.add_argument('target', type=Path, help='new .toolchain path in the test worktree')
    parser.add_argument('--qemu-i386', default='qemu-i386', help='executable name or path')
    args = parser.parse_args()
    qemu = shutil.which(args.qemu_i386)
    if not qemu:
        parser.error(f'cannot find {args.qemu_i386}; provide a working qemu-i386 executable')
    target = args.target.parent.resolve() / args.target.name
    try:
        prepare(args.source.resolve(), target, Path(qemu).absolute())
    except (OSError, ValueError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f'{parser.prog}: {exc}\n')
    print(f'Prepared {target}')


if __name__ == '__main__':
    main()
