#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad[3];
};

struct Unk9FA94 {
    u8 unk00[0x5C];
    s32 unk5C;
    u8 unk60[0x2C];
    s32 unk8C;
    s32 unk90;
    s32 unk94;
};

extern u8 D_800A3948[];
extern u8 D_8009C1BC[];
extern u8 D_8009C1AC[];
extern struct Entry D_8009B4E4[];
extern s8 D_8009B72C[];
extern struct Unk9FA94 *D_8009FA94;
extern u8 D_8009F3C0;
extern u8 D_8009F3C1;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F3F1;
extern u8 D_8009F458;
extern u8 D_8009F880;
extern u8 D_8009F881;
extern u8 D_8009F882;

extern void func_80077FFC(void);
extern void func_80077708(void);
extern void func_800776B8(void);
extern void func_8007A168(void);

#define CLAMPN(x, m) do { if ((s8)(x) >= 0) { if ((s8)(x) > (m)) { t = (m); } else { t = (x); } } else { t = 0; } (x) = t; } while (0)

s32 func_8007EA78(void)
{
    u8 buf[12];
    u32 i;
    u32 j;
    register s32 ret __asm__("$22");
    u8 c;
    register u8 b3 __asm__("$3");
    u8 b4, b6;
    register u8 b1 __asm__("$4");
    register u8 b5 __asm__("$5");
    register s32 b7 __asm__("$10");
    register u32 b7i __asm__("$8");
    register u8 b0 __asm__("$9");
    register u8 b9 __asm__("$11");
    register u8 b2 __asm__("$7");
    register u8 b8 __asm__("$6");
    register s32 n4 __asm__("$16");
    register s32 n5 __asm__("$17");
    register s32 c0 __asm__("$19");
    register s32 c2 __asm__("$20");
    register s32 c1 __asm__("$21");
    register s32 n0 __asm__("$4");
    register s32 n1 __asm__("$24");
    register s32 n2 __asm__("$15");
    register s32 n3 __asm__("$25");
    s32 r1, r4;
    register s32 ctmp __asm__("$2");
    register s32 ctmp2 __asm__("$3");
    register s32 f1 __asm__("$14");
    register s32 f2 __asm__("$13");
    register s32 f3 __asm__("$12");
    s32 f4, f5, f6;
    register s32 f7 __asm__("$18");
    register u32 g1 __asm__("$6");
    register u32 g2 __asm__("$5");
    register u32 g3 __asm__("$9");
    u8 g4, g5, g6;
    register u32 g7 __asm__("$7");
    s32 sum;
    register s32 sum2 __asm__("$2");
    u8 v;
    register s32 t __asm__("$3");

    n5 = 0;
    ret = 0;
    __asm__ volatile("" : "+r"(n5), "+r"(ret) :: "memory");
    for (i = 0; i < 10; i++) {
        c = D_800A3948[i] + 0x41;
        if (i & 1) {
            for (j = 0; j < 8; j++) {
                if (c == D_8009C1BC[j]) {
                    buf[i] = (j - i) & 7;
                    break;
                }
            }
            if (j == 8) {
                break;
            }
        } else {
            for (j = 0; j < 16; j++) {
                if (c == D_8009C1AC[j]) {
                    buf[i] = (j - i) & 0xF;
                    break;
                }
            }
            if (j == 16) {
                break;
            }
        }
    }
    if (i < 10) {
        return ret;
    }

    b4 = buf[4]; b6 = buf[6];
    __asm__ volatile("" : : "r"(b4), "r"(b6));
    b0 = buf[0]; b9 = buf[9];
    __asm__ volatile("" : : "r"(b0), "r"(b9));
    b2 = buf[2]; b8 = buf[8];
    __asm__ volatile("" : : "r"(b2), "r"(b8));
    b1 = buf[1]; b5 = buf[5];
    __asm__ volatile("" : : "r"(b1), "r"(b5));
    b7 = buf[7];
    __asm__ volatile("" : "+r"(b7));

    c0 = b0 & 3;
    __asm__ volatile("" : "+r"(c0));
    ctmp = b9 >> 1;
    __asm__ volatile("" : "+r"(ctmp));
    c1 = ctmp & 3;
    __asm__ volatile("" : : "r"(c1));
    ctmp2 = b2 & 3;
    __asm__ volatile("" : "+r"(ctmp2));
    ctmp = b8 & 0xC;
    __asm__ volatile("" : "+r"(ctmp));
    c2 = ctmp2 | ctmp;
    ctmp = b1 >> 2;
    __asm__ volatile("" : "+r"(ctmp));
    f1 = ctmp & 1;
    __asm__ volatile("" : "+r"(f1));
    f2 = b5 & 1;
    __asm__ volatile("" : "+r"(f2));
    b7i = (u8)b7;
    __asm__ volatile("" : "+r"(b7i));
    ctmp = b7i >> 2;
    __asm__ volatile("" : "+r"(ctmp));
    f3 = ctmp & 1;
    __asm__ volatile("" : "+r"(f3));
    n0 = b1 & 3;
    r1 = ((b2 >> 2) & 3) | ((b8 & 3) << 2);
    n1 = r1;
    r4 = ((b0 >> 2) & 3) | ((b5 << 1) & 0xC);
    n4 = r4;
    f4 = b7 & 1;
    f5 = (b7i >> 1) & 1;
    b3 = buf[3];
    f6 = b9 & 1;
    n0 |= (b3 & 3) << 2;
    f7 = (b3 >> 2) & 1;
    n2 = b4;
    n3 = b6;

    if (r1 >= 10) {
        if ((s8)n2 >= 10 && (s8)n3 == 10 && r4 == 10) {
            ctmp = r1 - 20;
            __asm__ volatile("" : "+r"(ctmp));
            n5 = n2 + ctmp;
            __asm__ volatile("" : "+r"(n4));
            n1 = 10;
            __asm__ volatile("" : "+r"(n1));
            n2 = n1;
        }
    }

    CLAMPN(n0, 8);
    CLAMPN(n1, 10);
    CLAMPN(n2, 10);
    CLAMPN(n3, 10);
    CLAMPN(n4, 10);
    CLAMPN(n5, 10);

    __asm__ volatile("" : "+r"(f7));
    sum = (s8)n0 + (s8)n1 + (s8)n2 + (s8)n3 + (s8)n4 + (s8)n5;
    if (c0 == (sum & 3)) {
        g1 = (u8)f1; sum2 = sum + g1;
        g2 = (u8)f2; sum2 += g2;
        g3 = (u8)f3; sum2 += g3;
        g4 = f4; sum2 += g4;
        g5 = f5; sum2 += g5;
        g6 = f6; sum2 += g6;
        g7 = (u8)f7; sum = sum2 + g7;
        if (c1 == (sum & 3)) {
            if (c2 != (sum & 0xF)) return ret; {
                if (g1) {
                    D_8009FA94->unk8C = g1;
                }
                if (g2) {
                    D_8009FA94->unk90 = g2;
                    D_8009FA94->unk94 = g3;
                }
                if (g7) {
                    D_8009F3C0 = f7;
                }
                if (g4) {
                    D_8009F3C1 = f4;
                }
                if (g5) {
                    D_8009F3C3 = f5;
                }
                if (g6) {
                    D_8009F3C4 = f6;
                }
                D_8009F458 = n0;
                D_8009F3F1 = n0;
                D_8009B4E4[D_8009B72C[0]].unk1F = n1;
                D_8009B4E4[D_8009B72C[1]].unk1F = n2;
                D_8009B4E4[D_8009B72C[2]].unk1F = n3;
                D_8009B4E4[D_8009B72C[3]].unk1F = n4;
                D_8009B4E4[D_8009B72C[4]].unk1F = n5;
                D_8009B4E4[D_8009B72C[0]].unk20 = n1;
                D_8009B4E4[D_8009B72C[1]].unk20 = n2;
                D_8009B4E4[D_8009B72C[2]].unk20 = n3;
                D_8009B4E4[D_8009B72C[3]].unk20 = n4;
                D_8009B4E4[D_8009B72C[4]].unk20 = n5;
                func_80077FFC();
                if (D_8009FA94->unk5C == 1) {
                    func_80077708();
                } else {
                    func_800776B8();
                    v = D_8009F458;
                    if ((s8)D_8009F458 >= 0) { if ((s8)D_8009F458 < 8) { } else { v = 7; } } else { v = 0; }
                    D_8009F882 = v;
                    D_8009F881 = 0;
                    D_8009F880 = v;
                }
                func_8007A168();
                ret = 1;
            }
        }
    }
    __asm__ volatile("" : "+r"(ret));
    return ret;
}
