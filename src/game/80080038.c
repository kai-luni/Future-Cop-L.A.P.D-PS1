#include "game/types.h"

struct Item {
    u8 pad00[4];
    u8 on;
    u8 pad05[0x35];
    u8 mode;
    u8 pad3F[1];
    u8 unk3E;
    u8 pad[0];
};

struct Ctx {
    u8 pad000[0x200];
    u8 st200;
    u8 st201;
    u8 st202;
    u8 st203;
    u8 st204;
    u8 st205;
    u8 st206;
    u8 pad207[0x424 - 0x207];
    u8 items[0x4FD];
    u8 st921;
    u8 pad922[0x20];
    u16 msg;
    s8 st944;
    s8 st945;
};

extern s8 *D_8009F468;
extern u8 D_8009F8BD;
extern s8 D_8009F8C0;
extern struct Ctx *D_8009F92C;
extern volatile s32 D_8009FA60;
extern s8 D_800A3718[];
extern s8 D_8009B724[];
extern s8 D_8009B72C[];
extern u8 D_8009B4E4[];
extern u16 D_8009C1C4[];
extern u8 D_8009F484[];

extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);
extern void func_8007F128(s32, s32, s32, s32, s32, s32);
extern void func_8007FB04(void);
extern void func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80082EF0(s32, s32, s32, s32, s32, s32, s32, s32);
extern s32 func_8008CD30(u8 *, u8 *, s32);

void func_80080038(s32 arg0)
{
    u16 msg;
    s32 cnt, j, x, wd;
    register s32 sel __asm__("$21");
    u8 *it;
    u8 *e;
    s8 *tbl;
    u8 buf[1];
    s32 neg;

    msg = 0;
    if (arg0 != 0) {
        return;
    }
    func_8007FB04();
    neg = -1;
    func_80082AB0(9, 0x24, 0x34, neg, neg, 0x1F4, 0x80, 1, 8);
    func_80082EF0(0x25, 0x9B, 0x67, 2, 0xA, 0x60, 0xC0, 0xFF);
    func_80082AB0(8, 0x93, 0x34, neg, neg, 0x1F4, 0x80, 1, 8);

    if (D_8009F468 == D_800A3718) {
        func_80078B34(0x44C, 0, 0);
        func_80078B34(0x10CC, 1, 0);
    } else if (D_8009F8BD != 0 && *D_8009F468 < 10) {
        if (D_8009F92C->st921 == 0) {
            func_80078B34(0x44C, 0, 0);
            {
                u8 *p = *D_8009F468 * 0x70 + (u8 *)D_8009F92C;
                s32 on = p[0x428];
                __asm__("" : : "r"(p), "r"(on));
                if (on != 0) {
                if (((D_8009FA60 >> 9) & 1) != 0) {
                    func_80078B34(0x3912, 0, 0);
                }
            }
            }
            func_80078B34(0x3908, 1, 0);
        } else {
            if (D_8009F8C0 != 0) {
                if (((D_8009FA60 >> 9) & 1) != 0) {
                    if (((D_8009FA60 >> 10) & 1) != 0) {
                        func_80078B34(0x6E5, 0, 0);
                    } else {
                        func_80078B34(0x6D6, 0, 0);
                    }
                    func_80078B34(0x6E0, 1, 0);
                } else {
                    func_80078B34(0x3926, 0, 0);
                    func_80078B34(0x10CC, 1, 0);
                }
            } else {
                func_80078B34(0x391C, 0, 0);
                func_80078B34(0x3908, 1, 0);
            }
        }
    }

    switch (D_8009F92C->st200) {
    case 1:
    case 6:
        msg = 0x3728;
        break;
    case 0:
        if (D_8009F92C->st203 == 3 && (u8)(D_8009F92C->st204 - 3) < 2) {
            msg = 0x3732;
        } else {
            if (D_8009F92C->st204 == 2) {
                __asm__("" : : : "$21");
                msg = 0x373C;
            } else if (D_8009F92C->st204 == 1) {
                msg = 0x3746;
            } else {
                __asm__("" : : : "$21");
                msg = 0x3750;
            }
        }
        func_80078B34(0x3908, 1, 0);
        break;
    case 2:
        if (D_8009F92C->st921 == 0) {
            if (D_8009F92C->st201 == 1) {
                msg = 0x375A;
            } else {
                switch (D_8009F92C->st205) {
                case 1:
                    break;
                case 2:
                    msg = 0x376E;
                    func_80078B34(0x3908, 1, 0);
                    break;
                case 3:
                    break;
                case 0:
                default:
                    msg = 0x3782;
                    func_80078B34(0x3908, 1, 0);
                    break;
                }
            }
        } else {
            if (D_8009F92C->st201 == 1) {
                msg = 0x375A;
            } else if (D_8009F92C->st201 == 2) {
                msg = 0x3796;
            } else if (D_8009F92C->st204 == 2) {
                if (D_8009F92C->st202 != 2) {
                    msg = 0x373C;
                    func_80078B34(0x3908, 1, 0);
                }
            } else if (D_8009F92C->st206 != 0) {
                msg = 0x37B4;
                func_80078B34(0x3908, 1, 0);
            } else {
                switch (D_8009F92C->st205) {
                case 1:
                    break;
                case 2:
                    msg = 0x37C8;
                    func_80078B34(0x3908, 1, 0);
                    break;
                case 3:
                    break;
                case 0:
                default:
                    D_8009F92C->msg = 0x37DC;
                    break;
                }
            }
        }
        break;
    case 3:
        switch (D_8009F92C->st201) {
        case 3:
            msg = 0x37E6;
            break;
        case 4:
            msg = 0x3732;
            break;
        case 0:
            msg = 0x37FA;
            break;
        }
        break;
    case 5:
        msg = 0x3804;
        break;
    case 4:
        msg = 0x380E;
        func_800788F0(0, 0x57, 0x6C, 0x10, 0x3818, 4, 8, D_8009F92C->st945);
        func_80078B34(0x3908, 1, 0);
        break;
    }

    if (msg != 0) {
        func_800788F0(0, 0x57, 0x5A, 0x10, msg, 4, 8, D_8009F92C->st945);
    }
    if (D_8009F92C->msg != 0) {
        if (D_8009F92C->msg == 0x385E) {
            D_8009F92C->st944 = 1;
        }
        func_800788F0(0, 0x57, 0xA3, 0x10, D_8009F92C->msg, 4, 8, D_8009F92C->st945);
    }

    if (D_8009F8BD != 0 && D_8009F468 != D_800A3718 && *D_8009F468 < 10) {
        it = D_8009F92C->items + *D_8009F468 * 0x70;
        if (it[4] != 0) {
            if (D_8009F92C->msg == 0) {
                s32 s0;
                u8 *name = it + 0x1C;
                if (it[0x4C] == 2) {
                    j = (s32)(name + 0xC);
                    s0 = 0x9C;
                } else {
                    j = 0;
                    s0 = 0xA1;
                }
                func_800788F0((s32)name, 0x57, s0, 0x10, 0, 4, 8, 0);
                s0 += 10;
                if (j != 0) {
                    func_800788F0(j, 0x57, s0, 0x10, 0, 4, 8, 0);
                    s0 += 10;
                }
                {
                    s32 k;
                    switch (it[0x3E]) {
                    case 2: k = 0; break;
                    case 1: k = 1; break;
                    case 0: k = 2; break;
                    case 3: k = 3; break;
                    default: k = -1; break;
                    }
                    if (k != -1) {
                        func_800788F0(0, 0x57, s0, 0x10, D_8009C1C4[k], 4, 8, 0);
                    }
                }
            }
            if (it[0x44] == 1) {
                tbl = D_8009B72C;
                sel = 0;
                if (*(s32 *)(it + 0x40) == 0x0A0A0A0A) {
                    cnt = 4;
                } else {
                    cnt = 3;
                }
            } else {
                u32 hi = 0x800A0000;
                sel = (u8)it[0x34];
                __asm__ ("" : "=r"(sel) : "0"(sel));
                __asm__ ("" : "+r"(hi) : "r"(sel));
                tbl = (s8 *)(hi - 18652);
                if (sel >= 0) {
                    if (sel < 8) cnt = sel;
                    else cnt = 7;
                } else {
                    cnt = 0;
                }
            }
            j = 0;
            x = cnt + 1;
            if (x != 0) {
                do {
                    e = D_8009B4E4 + tbl[j] * 0x24;
                    if (j == sel && it[0x44] == 0) {
                        func_800788F0(0, 0x97, 0xB1, 0x10, *(u16 *)(e + 0x10), 1, 8, 2);
                    }
                    if (j < sel) {
                        wd = *(u16 *)(e + 0x12);
                    } else {
                        wd = *(u16 *)(e + 0x14);
                    }
                    if (it[0x44] == 1) {
                        s32 n, m;
                        register u8 *fmt __asm__("$5");
                        if (j == 4) {
                            n = it[0x4A] + 1;
                            if (n > 0) {
                                if (n < 11) {
                                    m = n;
                                } else {
                                    m = 10;
                                }
                            } else {
                                m = 1;
                            }
                            fmt = D_8009F484;
                            __asm__("" : : "r"(fmt), "r"(m) : "$4");
                            func_8008CD30(buf, fmt, m);
                            if (it[0x4A] == 10) {
                                wd = *(u16 *)(e + 0x12);
                            }
                        } else {
                            n = it[j + 0x40] + 1;
                            if (n > 0) {
                                if (n < 11) {
                                    m = n;
                                } else {
                                    m = 10;
                                }
                            } else {
                                m = 1;
                            }
                            fmt = D_8009F484;
                            __asm__("" : : "r"(fmt), "r"(m) : "$4");
                            func_8008CD30(buf, fmt, m);
                            if (it[j + 0x40] == 10) {
                                wd = *(u16 *)(e + 0x12);
                            }
                        }
                        func_800788F0((s32)buf, *(u16 *)(e + 0x1C), e[0x1E], 0x10, 0, 4, 0x20, 0);
                    }
                    func_80082AB0(wd, *(u16 *)(e + 0x16), e[0x18], -1, -1, 0xF, 0x80, 1, 8);
                    j++;
                } while (j < x);
            }
        }
    }

    if (D_8009F92C->st944 != 0) {
        func_800788F0(0, 0xA0, 0x1B, 0x10, 0x3AB6, 4, 8, 2);
        func_8007F128(0xA0, 0x25, 0xF, 2, 4, 8);
    }
}
