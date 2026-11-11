#include "game/types.h"

typedef struct Entry { s32 a, b; char name[16]; } Entry;
extern s32 D_8009CBC4, D_8009C8BC, D_8009C8B0;
extern Entry D_800A3E80[];
extern char D_800A3E88[];
extern char D_800125AC[], D_800125C8[], D_800125E0[], D_800125FC[], D_8001261C[], D_80012628[];
extern s32 func_80090218(void);
extern s32 func_800904DC(s32, char *);
extern s32 func_80090580(s32);
extern s32 func_800901F8(char *, char *);
extern void func_8008E000(char *, ...);

Entry *func_8008FF20(Entry *out, char *path)
{
    char buf[32];
    register char *p __asm__("$16");
    register char *q __asm__("$17");
    register s32 count __asm__("$18");
    register s32 slash __asm__("$21");
    register s32 neg __asm__("$20");
    register char *orig __asm__("$19");
    register Entry *dest __asm__("$22");
    s32 dir;
    register char *base __asm__("$2");
    register s32 off __asm__("$17");
    register Entry *entry __asm__("$16");
    register char *name __asm__("$19");
    orig = path;
    if (D_8009CBC4 != D_8009C8BC) {
        if (!func_80090218()) return 0;
        D_8009CBC4 = D_8009C8BC;
    }
    dest = out;
    if (*(signed char *)orig != '\\') return 0;
    buf[0] = 0;
    __asm__ volatile("");
    dir = 1;
    p = orig;
    count = 0;
    slash = '\\';
    neg = -1;
    do {
        s32 ch = *(unsigned char *)p;
        register s32 inner_slash __asm__("$5");
        register s32 c2 __asm__("$2");
        q = buf;
        if ((c2 = *(signed char *)p) != slash) {
            inner_slash = '\\';
            do {
                if (!ch) goto end;
                p++;
                *q = ch;
                ch = *(unsigned char *)p;
                q++;
            } while (*(signed char *)p != inner_slash);
            __asm__ volatile("lb\t%0,0(%1)\n\tnop" : "=r"(c2) : "r"(p));
        }
        if (!c2) goto end;
        p++;
        *q = 0;
        dir = func_800904DC(dir, buf);
        if (dir == neg) { buf[0] = 0; break; }
        count++;
    } while (count < 8);
end:
    if (count >= 8) {
        if (D_8009C8B0 > 0) func_8008E000(D_800125AC, orig, count);
        return 0;
    }
    if (!*(signed char *)buf) {
        if (D_8009C8B0 > 0) {
            register char *a1 __asm__("$5") = orig;
            __asm__ volatile("" : "=r"(a1) : "0"(a1));
            {
                register char *a0 __asm__("$4") = (char *)0x80010000;
                __asm__ volatile("" : "=r"(a0) : "0"(a0));
                func_8008E000(a0 + 0x25C8, a1);
            }
            __asm__ volatile(".set reorder");
        }
        return 0;
    }
    *q = 0;
    if (!func_80090580(dir)) {
        if (D_8009C8B0 <= 0) return 0;
        __asm__ volatile(".set\tnoreorder\n\tlui $4,%%hi(D_800125E0)\n\tjal func_8008E000\n\taddiu $4,$4,%%lo(D_800125E0)\n\t.set\treorder"
            : : : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory");
        return 0;
    }
    count = 0;
    if (D_8009C8B0 >= 2) {
        func_8008E000(D_800125FC, buf);
        __asm__ volatile("" : "=r"(count));
        count = 0;
    }
    base = D_800A3E88;
    __asm__("" : "=r"(base) : "0"(base));
    entry = (Entry *)(base - 8);
    name = base;
    off = 0;
    do {
        s32 hi = 0x800A0000;
        __asm__ volatile("" : "=r"(hi) : "0"(hi));
        if (!*(signed char *)(hi + off + 0x3E88)) break;
        if (func_800901F8(name, buf)) {
            if (D_8009C8B0 >= 2) {
                __asm__ volatile("");
                func_8008E000(D_8001261C, buf);
            }
            *dest = *entry;
            return entry;
        }
        entry++;
        name += 24;
        count++;
        off += 24;
    } while (count < 64);
    if (D_8009C8B0 > 0) {
        func_8008E000(D_80012628, buf);
        __asm__ volatile(".set reorder");
    }
    return 0;
}
