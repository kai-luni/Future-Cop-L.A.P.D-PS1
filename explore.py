import subprocess, os, re

REPO = '/home/kai-luni/git/future-cop-lapd-ps1'
ENV = os.environ.copy()
ENV['PATH'] = REPO + '/.toolchain/mipsel-binutils/root/usr/bin:' + ENV.get('PATH', '')
ENV['LD_LIBRARY_PATH'] = REPO + '/.toolchain/mipsel-binutils/root/usr/lib/x86_64-linux-gnu'

def try_c(body, name='test', verbose=True):
    full_c = f'''
#include "game/types.h"
struct Poly {{
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
}};
extern struct Poly *D_8009F9F0;
extern struct Poly *D_8009FA28;

void func_80082EF0(s16 x, s16 y, s16 w, s16 h, u32 link, s32 r, s32 g, s32 b)
{{
{body}
}}
'''
    with open('tmp_exp.c', 'w') as f:
        f.write(full_c)
    subprocess.run([REPO + '/.toolchain/gcc-2.7.2-cdk/cpp', '-lang-c', '-I', REPO + '/include',
                    'tmp_exp.c', 'tmp_exp.i'], env=ENV, check=True)
    subprocess.run([REPO + '/.toolchain/gcc-2.7.2-cdk/cc1', '-quiet', '-msoft-float', '-mgas', '-G0',
                    '-fgnu-linker', '-O2', '-mmips-as', '-O2', 'tmp_exp.i', '-o', 'tmp_exp.s'], env=ENV, check=True)
    with open('tmp_exp_asm.s', 'w') as f:
        f.write('.set D_8009F9F0, 0x8009F9F0\n.set D_8009FA28, 0x8009FA28\n')
        subprocess.run(['python3', REPO + '/.toolchain/maspsx-423ffc6bca05e53f9b7ab06159d0cbe71111bc90/maspsx.py',
                        '--dont-expand-li', 'tmp_exp.s'], stdout=f, env=ENV, check=True)
    subprocess.run(['mipsel-linux-gnu-as', '-EL', '-march=r3000', '-mabi=32', '-o', 'tmp_exp.o', 'tmp_exp_asm.s'], env=ENV, check=True)
    subprocess.run(['mipsel-linux-gnu-objcopy', '-O', 'binary', '-j', '.text', 'tmp_exp.o', 'tmp_exp.bin'], env=ENV, check=True)
    actual = open('tmp_exp.bin', 'rb').read()
    
    rom = open(REPO + '/baseroms/us/SLUS_007.39', 'rb').read()
    retail = rom[0x800 + 0x80082EF0 - 0x80010000:][:0xB4]
    
    eq = sum(1 for i in range(len(retail)//4) if actual[4*i:4*i+4] == retail[4*i:4*i+4])
    if verbose:
        print(f'{name:20s}: {eq}/45 words, len={len(actual)}')
        p1 = subprocess.run(['mipsel-linux-gnu-objdump', '-D', '-b', 'binary', '-m', 'mips:3000', '-EL',
                             '--adjust-vma=0x80082ef0', 'tmp_exp.bin'], capture_output=True, text=True, env=ENV)
        lines1 = p1.stdout.splitlines()[7:25]
        p2 = subprocess.run(['mipsel-linux-gnu-objdump', '-D', '-b', 'binary', '-m', 'mips:3000', '-EL',
                             '--adjust-vma=0x80082ef0', 'tmp_retail.bin'], capture_output=True, text=True, env=ENV)
        lines2 = p2.stdout.splitlines()[7:25]
        for l2, l1 in zip(lines2, lines1):
            m = ' ' if l2[24:] == l1[24:] else '!'
            print(f'{l2[10:35]:25s} | {l1[10:35]:25s} {m}')
    return eq, actual
