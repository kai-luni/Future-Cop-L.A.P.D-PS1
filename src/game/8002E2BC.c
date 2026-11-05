#include "game/types.h"

struct Unk2E2BC28 {
    u8 pad00[0x10];
    s32 unk10;
};

struct Unk2E2BC {
    u8 pad00[0x24];
    u32 unk24;
    struct Unk2E2BC28 *unk28;
    u8 pad2C[0x78 - 0x2C];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 pad88[0xC8 - 0x88];
    u8 unkC8[0x14];
    u8 unkDC;
    u8 unkDD;
    s16 unkDE;
    s16 unkE0;
    s16 unkE2;
    u8 padE4[4];
    s32 unkE8;
    u8 padEC[0x100 - 0xEC];
    s32 unk100;
    s32 unk104;
    s32 unk108;
    u8 pad10C[4];
};

extern s32 func_80019668(void);
extern void func_800448D8(void *track, s32 index, s32 direction, s32 flags);

void func_8002E2BC(struct Unk2E2BC *objArg, s32 modeArg)
{
    register struct Unk2E2BC *obj __asm__("$17") = objArg;
    register s32 mode __asm__("$16") = modeArg;
    s32 v;
    s32 sh;
    s32 x;
    register s32 t __asm__("$2");
    register u32 m __asm__("$3");
    u8 arg;

    if (obj->unkDC != mode) {
        if (obj->unkDD & 2) {
            if (mode == 0) {
                obj->unk24 |= 0x81;
            } else if (obj->unkDC == 0) {
                m = ~0x80;
                __asm__("" : "+r"(m));
                t = obj->unk24 & m;
                obj->unk24 = t & ~1;
            }
        }
        if (obj->unkDC == 2 && (*(u32 *)&obj->unkDC & 0x1400)) {
            obj->unk78 = obj->unk100;
            obj->unk7C = obj->unk104;
            obj->unk80 = obj->unk108;
        }
        switch (mode) {
        case 1:
            if (obj->unk28->unk10) {
                arg = ((u8 *)obj)[0x10D];
                goto call;
            }
            goto tail;
        case 2:
            if (obj->unk28->unk10) {
                func_800448D8(obj->unkC8, ((u8 *)obj)[0x10F], 1, 1);
            }
            sh = (u16)obj->unkE0 << 16;
            x = sh >> 16;
            obj->unkDC = mode;
            if (x <= 0) {
                goto zero;
            }
            v = x;
            if (obj->unkDD & 0x20) {
                goto store;
            }
            __asm__ volatile ("");
            goto rand;
        case 3:
            if (obj->unk28->unk10) {
                arg = ((u8 *)obj)[0x10E];
            call:
                func_800448D8(obj->unkC8, arg, 1, 0);
            }
        tail:
            t = obj->unkE8;
            obj->unkDC = mode;
            obj->unkE2 = 0x4650;
            obj->unk84 = t - ((t >> 31) & t);
            break;
        case 0:
            __asm__("" : "+r"(mode));
            if (obj->unk28->unk10) {
                func_800448D8(obj->unkC8, ((u8 *)obj)[0x10C], 1, 0);
            }
            sh = (u16)obj->unkDE << 16;
            x = sh >> 16;
            obj->unkDC = 0;
            if (x <= 0) {
                goto zero;
            }
            v = x;
            if (obj->unkDD & 0x20) {
                goto store;
            }
            if (v < 2) {
                goto store;
            }
        rand:
            v += (u32)func_80019668() % (u32)(sh >> 17);
        store:
            obj->unkE2 = v;
            goto done;
        zero:
            obj->unkE2 = 0;
        done:
            obj->unk84 = 0;
            break;
        }
    }
}
