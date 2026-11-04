#include "game/types.h"

typedef struct {
    u8 pad00[0x1A6];
    volatile u16 field1A6;
    u8 pad1A8[2];
    volatile u16 field1AA;
} SpuDebug;

extern SpuDebug *D_8009EEA4;
extern s32 D_8009EECC;
extern u16 D_8009EEBC;
extern s32 D_8009EEF4;
extern s32 D_8009EEF8;
extern s32 D_8009EEFC;
extern s32 *D_8009EEA8;
extern s32 *D_8009EEAC;
extern s32 *D_8009EEB0;

extern void func_80097940(void);
extern void func_80097968(void);

s32 func_8009745C(s32 arg0, ...)
{
    s32 *vl;
    u16 target;
    u32 poll;
    u32 want;
    u32 i;
    register u32 rx __asm__("$4");
    register s32 rs __asm__("$2");
    register SpuDebug *rp __asm__("$3");
    register s32 *re __asm__("$5");
    register u32 rq __asm__("$3");
    register u32 rm __asm__("$2");
    register s32 rf __asm__("$3");
    u32 val;

    vl = (s32 *)((u32)&arg0 + 4);
    switch (arg0) {
    case 2:
        rx = (u32)*vl;
        rs = D_8009EECC;
        rp = D_8009EEA4;
        poll = rx >> rs;
        D_8009EEBC = poll;
        rp->field1A6 = poll;
        break;
    case 1:
        D_8009EEF4 = 0;
        want = D_8009EEBC;
        i = 0;
        while (D_8009EEA4->field1A6 != want) {
            if (++i >= 0xF01) {
                return -2;
            }
        }
        D_8009EEA4->field1AA = (D_8009EEA4->field1AA & 0xFFCF) | 0x20;
        break;
    case 0:
        D_8009EEF4 = 1;
        want = D_8009EEBC;
        i = 0;
        while (D_8009EEA4->field1A6 != want) {
            if (++i >= 0xF01) {
                return -2;
            }
        }
        D_8009EEA4->field1AA |= 0x30;
        break;
    case 3:
        target = (D_8009EEF4 == 1) ? 0x30 : 0x20;
        i = 0;
        while ((D_8009EEA4->field1AA & 0x30) != target) {
            if (++i >= 0xF01) {
                return -2;
            }
        }
        if (D_8009EEF4 == 1) {
            func_80097968();
        } else {
            func_80097940();
        }
        vl = vl + 1;
        rx = (u32)vl[-1];
        D_8009EEF8 = rx;
        rx = (u32)*vl;
        re = D_8009EEA8;
        rq = rx >> 6;
        rm = rx & 0x3F;
        D_8009EEFC = rq + (rm != 0);
        *D_8009EEA8 = D_8009EEF8;
        *D_8009EEAC = (D_8009EEFC << 16) | 0x10;
        rf = D_8009EEF4;
        val = (rf == 1) ? 0x1000200 : 0x1000201;
        *D_8009EEB0 = val;
        break;
    }
    return 0;
}
