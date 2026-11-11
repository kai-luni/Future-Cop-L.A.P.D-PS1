#include "game/types.h"


extern volatile u8 D_8009FA3C;
extern u8 D_800A63E0[];
extern u8 D_800A0520[];
extern u8 D_800A0528[];
extern u8 D_800A0660[];
extern u32 D_8009AA68[];
extern s32 D_8009FA0C;

extern u32 D_8009F5C8[];
extern u32 D_8009F5D0[];
extern u32 D_8009F5D8[];

extern s32 D_8009F218;
extern s32 D_8009F21C;
extern s32 D_8009F220;
extern s32 D_8009F5C0;
extern u32 D_8009F5E0;
extern u32 D_8009F5E4;
extern s32 D_8009F5E8;
extern s32 D_8009F5EC;
extern s32 D_8009F5F0;
extern s32 D_8009F5F4;
extern s32 D_8009F5F8;
extern s32 D_8009F5FC;
extern s32 D_8009F600;
extern u32 D_8009F9F0;
extern u32 D_8009FA90;
extern s32 D_8009F9D4;
extern u32 D_8009F9C0;
extern u32 D_8009FA28;

extern u8 func_8001C034(void);
extern s32 func_80095C14(s32 value);
extern void func_80029650(u32 arg0);
extern void func_80092F38(u32 arg0, u32 arg1);
extern void func_80092C8C(s32 arg0);
extern void func_80093178(u32 arg0, s32 arg1);
extern void func_80093280(u32 arg0);
extern void func_8009398C(u32 arg0, void *arg1);
extern void func_80093A0C(u32 arg0, void *arg1);
extern void func_800861E4(u32 arg0, u32 arg1, u32 arg2, u32 arg3);

void func_8001B4A8(s32 arg0)
{
    register s32 max __asm__("$16");
    register u32 q __asm__("$17");
    register u32 p __asm__("$18");
    register s32 min __asm__("$19");
    register u32 limY __asm__("$20");
    register u32 limX __asm__("$21");
    register u32 h __asm__("$22");
    register u32 table __asm__("$23");
    s32 count;
    register s32 v __asm__("$2");
    register s32 xbase __asm__("$3");
    register s32 shift __asm__("$7");
    register u32 mask __asm__("$6");
    register s32 arg9 __asm__("$9");

    u32 pcur;
    u32 qcur;
    register u32 av __asm__("$5");
    register u32 bv __asm__("$4");
    register u32 val __asm__("$8");
    u32 ptr;
    s32 quot;
    s32 idx;
    s16 rect2[2];
    s16 rect[4];
    u32 e;
    u32 hnd;
    s32 dx;
    s32 dy;

    while (D_8009FA3C) {
    }

    __asm__("" : "+m"(arg0));
    h = func_8001C034();
    if (h != 0) {
        h = 0x100;
    }

    xbase = D_8009F9F0;
    v = 0x100 - h;
    D_8009F9D4 = v;
    v = D_8009FA90;
    limX = D_8009F5E0;
    limY = D_8009F5E4;

    p = xbase - 0x34;
    pcur = p;
    q = v - 0x1C;
    qcur = q;

    if (p < limX && q < limY) {
        goto done;
    }

    max = 0xFFC00000;
    min = 0x400000;

    if (p >= limX) {
        do {
            v = *(s32 *)pcur;
            pcur -= 0x34;
            xbase = v & 0xFFFFFF;
            if (xbase != 0) {
                if (max < xbase) {
                    max = xbase;
                }
                if (xbase < min) {
                    min = xbase;
                }
            }
        } while (pcur >= limX);
    }

    if (qcur >= limY) {
        do {
            v = *(s32 *)qcur;
            qcur -= 0x1C;
            xbase = v & 0xFFFFFF;
            if (xbase != 0) {
                if (max < xbase) {
                    max = xbase;
                }
                if (xbase < min) {
                    min = xbase;
                }
            }
        } while (qcur >= limY);
    }

    v = max - 1;
    quot = (D_8009F5FC + v - min) / D_8009F5FC;
    shift = 1;
    if (quot != 0) {
        v = func_80095C14(quot);
        xbase = 0x20;
        shift = xbase - v;
    }

    if (max != 0xFFC00000) {
        v = max - min;
        v = v >> shift;
        count = v + 1;
    } else {
        count = 0;
    }

    mask = 0xFF0000;
    v = D_8009F600;
    __asm__("" : "+r"(mask) : "r"(v));
    mask |= 0xFFFF;
    xbase = (s32)D_8009F5D8;
    v = v << 2;
    v = v + xbase;
    table = *(u32 *)v;

    if (q >= limY) {
        do {
            v = *(u32 *)q;
            max = v & mask;
            v ^= max;
            if (max != 0) {
                max = ((max - min) >> shift) + 1;
            }
            xbase = max << 2;
            xbase = xbase + table;
            av = *(u32 *)xbase;
            bv = av & mask;
            av ^= bv;
            v = v | bv;
            *(u32 *)q = v;
            v = q & mask;
            q -= 0x1C;
            v = av | v;
            *(u32 *)xbase = v;
        } while (q >= limY);
    }

    if (p >= limX) {
        do {
            v = *(u32 *)p;
            max = v & mask;
            v ^= max;
            if (max != 0) {
                max = ((max - min) >> shift) + 1;
            }
            xbase = max << 2;
            xbase = xbase + table;
            av = *(u32 *)xbase;
            bv = av & mask;
            av ^= bv;
            v = v | bv;
            *(u32 *)p = v;
            v = p & mask;
            p -= 0x34;
            v = av | v;
            *(u32 *)xbase = v;
        } while (p >= limX);
    }

    if (D_8009FA0C >= 2 || ({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; })) {
        arg9 = arg0;
        limX = 0;
        if (arg9 == 0 && D_8009F5C0 != 0) {
            limX = 0xA0;
        }

        rect[0] = limX;
        rect[1] = h;
        rect[2] = (arg9 != 0) ? 0x140 : 0xA0;
        rect[3] = 0xF0;

        q = (u32)D_800A63E0;
        func_8009398C(q, rect);

        p = 0xFFFFFF;
        mask = q & p;
        __asm__ volatile("" : "+r"(q) : "r"(count));
        q += 0x1C;
        __asm__("" : "+r"(q) : "r"(count));
        bv = q;
        __asm__("" : : "r"(bv));
        max = (count << 2) + table;
        min = 0xFF000000;
        xbase = *(u32 *)D_800A63E0;
        v = *(u32 *)(max + 4);
        *(u32 *)D_800A63E0 = (xbase & min) | (v & p);
        v = *(volatile u32 *)(max + 4);
        *(u32 *)(max + 4) = (v & min) | mask;

        rect2[0] = limX;
        rect2[1] = h;
        func_80093A0C(q, rect2);

        xbase = *(u32 *)q;
        v = *(u32 *)(max + 4);
        *(u32 *)q = (xbase & min) | (v & p);
        v = *(volatile u32 *)(max + 4);
        *(u32 *)(max + 4) = (v & min) | (q & p);
    }

    if (D_8009F21C != D_8009F220) {
        do {
            v = (u32)D_800A0520 + (D_8009F21C << 4);
            hnd = *(u32 *)(v + 4);
            if (hnd != 0) {
                func_80029650(hnd);
            }
            D_8009F21C++;
            if (D_8009F21C >= 0x14) {
                D_8009F21C = 0;
            }
        } while (D_8009F21C != D_8009F220);
    }

    if (D_8009F220 != D_8009F218) {
        ptr = (u32)D_800A0528;
        q = ptr - 8;
        do {
            bv = D_8009F220 << 4;
            v = bv + q;
            av = *(u32 *)v;
            func_80092F38(bv + ptr, av);
            func_80092C8C(0);
            D_8009F220++;
            if (D_8009F220 >= 0x14) {
                D_8009F220 = 0;
            }
        } while (D_8009F220 != D_8009F218);
    }

    if (D_8009AA68[0] != 0) {
        func_80092F38((u32)D_8009AA68 + 8, D_8009AA68[0]);
        func_80092C8C(0);
        if (D_8009AA68[1] != 0) {
            func_80029650(D_8009AA68[1]);
        }
        D_8009AA68[0] = 0;
    }

    if (({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; }) || D_8009F5C0 == 0 || D_8009FA0C == 1) {
        if (D_8009F5EC != D_8009F5F0) {
            do {
                v = (u32)D_800A0660 + D_8009F5EC * 20;
                hnd = *(u32 *)(v + 4);
                if (hnd != 0) {
                    func_80029650(hnd);
                }
                D_8009F5EC++;
                if (D_8009F5EC >= 4) {
                    D_8009F5EC = 0;
                }
            } while (D_8009F5EC != D_8009F5F0);
        }

        if (D_8009F5F0 != D_8009F5E8) {
            do {
                xbase = (u32)D_800A0660 + D_8009F5F0 * 20;
                func_800861E4(*(u32 *)xbase, *(u8 *)(xbase + 8),
                              *(u32 *)(xbase + 0xC), *(u32 *)(xbase + 0x10));
                func_80092C8C(0);
                D_8009F5F0++;
                if (D_8009F5F0 >= 4) {
                    D_8009F5F0 = 0;
                }
            } while (D_8009F5F0 != D_8009F5E8);
        }
    }

    max = count + 1;
    func_80092C8C(0);
    func_80093280(table + (max << 2));

    if (({ arg9 = arg0; __asm__ volatile("" : "+r"(arg9)); arg9 != 0; })) {
        D_8009FA3C = 1;
    }

    mask = 1;
    v = D_8009F600;
    __asm__ volatile("" : "+r"(mask) : "r"(v));
    xbase = (s32)D_8009F5D0;
    mask -= v;
    av = mask << 2;
    v = (s32)D_8009F5D8 + av;
    bv = *(u32 *)v;
    __asm__ volatile("" : : "r"(v));
    v = (s32)D_8009F5C8 + av;
    av += xbase;
    shift = *(u32 *)v;
    val = *(u32 *)av;
    __asm__("" : : "r"(av));
    av = D_8009F5FC;
    xbase = D_8009F5F4;
    D_8009F600 = mask;
    v = xbase * 52;
    xbase = D_8009F5F8;
    __asm__("" : : "r"(xbase));
    v -= 0x34;
    D_8009F5E0 = shift;
    D_8009F9F0 = shift;
    shift += v;
    D_8009F5E4 = val;
    D_8009FA90 = val;
    D_8009FA28 = shift;
    v = xbase * 28;
    v -= 0x1C;
    val += v;
    D_8009F9C0 = val;
    func_80093178(bv, av + 2);
done:
    return;
}
