#include "game/types.h"

typedef struct {
    u8 pad0[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad21[3];
} Entry;

extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
typedef struct { u8 b[0x70]; } Rec;
typedef struct {
    u8 pad0[0x424]; Rec recs[10]; u8 pad884[0x9D];
    u8 mode; u8 pad922[2]; u8 index; u8 name[0x1D]; u16 timer;
} Big;
extern Big *D_8009F92C;
#define RAW ((u8 *)D_8009F92C)
extern s32 D_8009F8B8;
extern u8 *D_8009F878;
extern s16 D_8009F87C;
extern s8 D_8009F882;
extern s8 D_8009F880;
extern s8 D_8009F881;
extern s8 D_8009F8C0[];
extern u8 D_800A3928[];
extern s32 D_8009F8F8[];
extern u8 D_800A6FE0[];
extern u8 *D_8009FA94;
extern u8 D_8009F458;
extern u8 D_8009F459;
extern u8 D_8009F3F1;
extern u8 D_8009F3C0;
extern u8 D_8009F3C1;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F3E6;
extern u8 D_8009F3E7;
extern u8 D_8009FA10;
extern u8 *D_8009F468;
extern Entry D_8009B4E4[];
extern s8 D_8009B72C[];
extern u8 D_800120BC[];
extern u8 D_800120C8[];
extern u8 D_800120D4[];
extern u8 D_8009F484[];
extern u16 D_8009F470;
extern u8 D_8009F498[];
extern u8 D_8009F4A0[];
extern u8 D_8009F4A4[];
extern u8 D_800A3718[];
extern u8 D_8009F3F8[];
extern s32 D_8009FA60;

extern void func_8007AA9C(u8 *a, u8 *b, s32 c, u8 *d);
extern void func_8007A9D0(u8 *a, u8 *b, s32 c, u8 *d, s32 e);
extern void func_80080B70(void);
extern void func_80080984(u8 *a, s32 b);
extern void func_8008CCB0(u8 *a, u8 *b);
extern s32 func_8008CCC0(u8 *a);
extern s32 func_8008CD30(u8 *a, u8 *fmt, ...);
extern u8 *func_80089914(s32 id);
extern void func_80078650(u8 *a, s32 b);
extern void func_80046C84(s32 id, s32 b);
extern void func_800492A0(u8 *text, s32 *w, s32 *h);
extern s32 *func_80048E88(void);
extern void func_80082EF0(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_800788F0(u8 *a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h);
extern void func_80077FFC(void);
extern void func_80077708(void);
extern void func_800776B8(void);
extern void func_8007FACC(void);
extern void func_80078210(void);

#define REC(off) (D_8009F92C->recs[D_8009F92C->index].b[(off) - 0x424])
#define RECP(base, off) ((base) + (base)[0x924] * 0x70 + (off))
#define CNT(e) \
    if ((e).unk1F + 1 > 0) { \
        if ((e).unk1F + 1 < 11) { n += 1 + (e).unk1F; } \
        else { n += 10; } \
    } else { n += 1; }

void func_8008125C(u32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 s2;
    s32 n;
    s32 i;
    u8 *p;
    u8 *q;
    u8 *name;
    u8 *s;
    register s32 lenv __asm__("$3");
    register s32 step __asm__("$2");
    s32 y;
    s32 w, h;
    s32 w2, h2;
    u8 txt[8];
    u8 *base;
    Big *init;
    register s32 heightTmp __asm__("$5");
    register s32 one __asm__("$7");
    s32 *cursor;
    register u8 *textBase __asm__("$16");
    register u8 *savedBase __asm__("$3");
    register s32 savedIdx __asm__("$2");
    register s32 off __asm__("$5");
    register s32 selected __asm__("$3");
    register u8 *dst __asm__("$4");

    __asm__("" : : "r"(arg4) : "$18");
    s2 = D_8009F8B8;
    if (D_8009F8BD != 0 && D_8009F8BE != 0 && arg0 < 6) {
        switch (arg0) {
        case 3:
            init = D_8009F92C;
            init->index = arg4;
            init->timer = 0;
            base = RAW;
            if (base[0x921] == 1) {
                if (D_8009F8C0[s2] != 0) {
                    D_8009F8C0[s2] = 0;
                    func_8007AA9C(RAW + 0x925, D_800A3928 + s2 * 8, 8, D_8009F878);
                    func_80080B70();
                    break;
                }
                selected = base[0x924];
                __asm__("" : "+r"(selected));
                step = (selected << 3) - selected;
                __asm__("" : "+r"(step));
                off = step << 4;
                if ((base + off)[0x428] == 0) {
                    s32 *fa = (s32 *)D_8009FA94;
                    s32 count;
                    count = fa[0x8C / 4] != 0;
                    if (fa[0x90 / 4] != 0) count++;
                    if (fa[0x94 / 4] != 0) count++;
                    if (count != 0) {
                        func_8008CD30(txt, D_8009F484, count);
                    } else {
                        *(u16 *)txt = D_8009F470;
                    }
                    if (*(s32 *)(D_8009FA94 + 0x5C) == 1) {
                        n = 0;
                        for (arg2 = 0; arg2 < 4; arg2++) {
                            CNT(D_8009B4E4[D_8009B72C[arg2]])
                        }
                        CNT(D_8009B4E4[D_8009B72C[4]])
                        arg2 = (s32)func_80089914(0x52D0);
                        if (((u8 *)arg2) != 0 && func_8008CCC0(((u8 *)arg2)) < 5) {
                            func_8008CD30(RAW + 0x925, D_800120BC, ((u8 *)arg2), n, txt);
                        } else {
                            func_8008CD30(RAW + 0x925, D_800120C8, n, txt);
                        }
                    } else {
                        arg2 = (s32)func_80089914(0x3868);
                        if (((u8 *)arg2) != 0 && func_8008CCC0(((u8 *)arg2)) < 7) {
                            func_8008CD30(RAW + 0x925, D_8009F498, ((u8 *)arg2), D_8009F882 + 1, txt);
                        } else {
                            func_8008CD30(RAW + 0x925, D_800120D4, D_8009F882 + 1, txt);
                        }
                    }
                } else {
                    dst = base + 0x925;
                    __asm__("addu %0,%0,%1" : "+r"(off) : "r"(base), "r"(dst));
                    func_8008CCB0(dst, (u8 *)off + 0x428);
                }
                D_8009F8F8[s2] = 0;
                func_8007A9D0(RAW + 0x925, D_800A3928 + s2 * 8, 8, D_8009F878, D_8009F87C);
                if (REC(0x428) != 0) {
                    RAW[0x200] = 5;
                    RAW[0x201] = 0;
                    RAW[0x204] = 0;
                    func_80078650(D_800A3718, 1);
                    *D_8009F468 = 1;
                } else {
                    D_8009F8C0[s2] = 1;
                }
                break;
            }
            if (RAW[arg4 * 0x70 + 0x428] == 0) {
                break;
            }
            func_80080984(RECP(RAW, 0x474), 0);
            textBase = D_800A6FE0;
            dst = textBase;
            savedBase = RAW;
            savedIdx = savedBase[0x924];
            off = savedIdx * 0x70;
            __asm__("addu %0,%0,%1" : "+r"(off) : "r"(savedBase), "r"(dst));
            func_8008CCB0(dst, (u8 *)off + 0x440);
            dst = textBase + 9;
            savedBase = RAW;
            savedIdx = savedBase[0x924];
            off = savedIdx * 0x70;
            __asm__("addu %0,%0,%1" : "+r"(off) : "r"(savedBase), "r"(dst));
            func_8008CCB0(dst, (u8 *)off + 0x44C);
            if (REC(0x459) != 0) *(s32 *)(D_8009FA94 + 0x8C) = REC(0x459);
            if (REC(0x45A) != 0) *(s32 *)(D_8009FA94 + 0x90) = REC(0x45A);
            if (REC(0x45B) != 0) *(s32 *)(D_8009FA94 + 0x94) = REC(0x45B);
            *(s32 *)(D_8009FA94 + 0x28) = REC(0x45C);
            *(s32 *)(D_8009FA94 + 0x34) = REC(0x45D);
            *(s32 *)(D_8009FA94 + 0x2C) = REC(0x45E);
            *(s32 *)(D_8009FA94 + 0x38) = REC(0x45F);
            *(s32 *)(D_8009FA94 + 0x30) = REC(0x460);
            *(s32 *)(D_8009FA94 + 0x3C) = REC(0x461);
            *(s32 *)(D_8009FA94 + 0x8) = REC(0x462);
            *(s32 *)(D_8009FA94 + 0x98) = REC(0x463);
            *(s32 *)(D_8009FA94 + 0xC) = REC(0x470);
            *(s32 *)(D_8009FA94 + 0x5C) = REC(0x468);
            D_8009F3E6 = REC(0x46A);
            D_8009F3E7 = REC(0x46B);
            if (REC(0x469) != 0) D_8009F3C0 = REC(0x469);
            if (REC(0x46C) != 0) D_8009F3C1 = REC(0x46C);
            if (REC(0x46D) != 0) D_8009F3C3 = REC(0x46D);
            if (REC(0x46F) != 0) D_8009F3C4 = REC(0x46F);
            for (arg2 = 0; arg2 < 4; arg2++) {
                D_8009B4E4[D_8009B72C[arg2]].unk1F = REC(0x464 + arg2);
                D_8009B4E4[D_8009B72C[arg2]].unk20 = REC(0x464 + arg2);
            }
            D_8009B4E4[D_8009B72C[4]].unk1F = REC(0x46E);
            D_8009B4E4[D_8009B72C[4]].unk20 = REC(0x46E);
            func_80077FFC();
            if (*(s32 *)(D_8009FA94 + 0x5C) == 1) {
                D_8009F459 = 0;
                func_80077708();
            } else {
                s32 v;
                s32 saved;
                func_800776B8();
                saved = REC(0x458);
                D_8009F458 = saved;
                D_8009F3F1 = REC(0x458);
                if ((s8)saved >= 0) {
                    if ((s8)saved >= 8) v = 7;
                    else v = saved;
                } else {
                    v = 0;
                }
                D_8009F882 = v;
                D_8009F881 = 0;
                D_8009F880 = v;
            }
            D_8009FA10 = 0;
            func_8007FACC();
            func_80078210();
            break;
        case 4:
            *(u16 *)(RAW + 0x942) = 0;
            if (D_8009F8C0[s2] != 0 && arg4 == RAW[0x924]) {
                func_80046C84(0x46, 0);
                arg2 = D_8009F8F8[s2];
                arg3 = (s32)(D_800A3928 + s2 * 8);
                p = (u8 *)arg3 + arg2;
                if (*p != 0) {
                    *p = *p - 1;
                } else {
                    *p = *(u8 *)&D_8009F87C - 1;
                }
            }
            break;
        case 5:
            *(u16 *)(RAW + 0x942) = 0;
            if (D_8009F8C0[s2] != 0 && arg4 == RAW[0x924]) {
                func_80046C84(0x46, 0);
                arg2 = D_8009F8F8[s2];
                arg3 = (s32)(D_800A3928 + s2 * 8);
                p = (u8 *)arg3 + arg2;
                *p = *p + 1;
                if (*p >= D_8009F87C) {
                    *p = 0;
                }
            }
            break;
        case 1:
            if (D_8009F8C0[s2] != 0 && arg4 == RAW[0x924]) {
                step = s2 << 2;
                __asm__("" : "+r"(step));
                cursor = (s32 *)(step + (u8 *)D_8009F8F8);
                *cursor = *cursor + 1;
                if (*cursor >= 8) {
                    *cursor = 0;
                }
                func_80046C84(0x47, 0);
            }
            break;
        case 2:
            if (D_8009F8C0[s2] != 0 && arg4 == RAW[0x924]) {
                step = s2 << 2;
                __asm__("" : "+r"(step));
                cursor = (s32 *)(step + (u8 *)D_8009F8F8);
                *cursor = *cursor - 1;
                if (*cursor < 0) {
                    *cursor = 7;
                }
                func_80046C84(0x47, 0);
            }
            break;
        case 0:
            y = arg2;
            if (D_8009F8C0[s2] != 0 && arg4 == RAW[0x924]) {
                s32 *font;
                s32 *pi;
                s32 col;
                s32 rgb;
                func_800492A0(D_8009F4A0, &w, &h);
                w = w + 1;
                arg2 = 0;
                font = func_80048E88();
                arg3 = (s32)(D_800A3928 + s2 * 8);
                s2 = (s32)&D_8009F8F8[s2];
                h = ((s16 *)font)[3];
                txt[1] = 0;
                n = arg1 - (w + 1) * 4;
                render_loop:
                {
                    txt[0] = D_8009F878[((u8 *)arg3)[arg2]];
                    func_800492A0(txt, &w2, &h2);
                    if (arg2 == *(s32 *)s2) {
                        dst = (u8 *)n;
                        one = 1;
                        savedBase = D_8009F3F8;
                        step = D_8009FA60;
                        heightTmp = h;
                        __asm__("sra %0,%0,4\n\tandi %0,%0,7\n\taddu %0,%0,%1"
                            : "+r"(step) : "r"(savedBase), "r"(heightTmp), "r"(dst), "r"(one));
                        selected = *(u8 *)step;
                        step = 10;
                        __asm__("" : "+r"(selected), "+r"(step));
                        selected += 0x80;
                        func_80082EF0((s32)dst, y + heightTmp + 2, w, one, step,
                                      selected, selected, selected);
                        col = 8;
                    } else {
                        col = 15;
                    }
                    arg2++;
                    func_80049100(txt, n + (w - w2) / 2, y, col);
                    lenv = w;
                    __asm__("addiu %0,%1,1" : "=r"(step) : "r"(n), "r"(lenv));
                    n = step + lenv;
                    if (arg2 < 8) goto render_loop;
                }
            } else {
                if (RAW[arg4 * 0x70 + 0x428] == 0) {
                    q = func_80089914(0x3714);
                    if (q == 0 || func_8008CCC0(q) >= 9) {
                        q = D_8009F4A4;
                    }
                } else {
                    q = RAW + arg4 * 0x70 + 0x428;
                }
                func_800788F0(q, arg1, arg2, arg3, 0, 4, 8, 0);
            }
            break;
        }
    }
}
