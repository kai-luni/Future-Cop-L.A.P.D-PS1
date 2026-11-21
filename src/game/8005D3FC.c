// Categories: Graphics > Effects | Game Engine > Object Setup
#include "game/types.h"

extern int gWeaponEffectUpdate();

struct Entity;

struct Sub {
    s32 x;
    s32 y;
    s32 scale;
    s16 angle;
    u8 pad_0E[2];
    s16 velocity;
    u8 pad_12[2];
    s32 timer;
    u8 enabled;
    u8 state;
    u8 pad_1A[2];
};

struct Block {
    void *unk00;
    struct Entity *unk04;
    s32 unk08;
    s32 unk0C;
    u8 unk10[4];
    s32 unk14;
    u8 unk18[0x30];
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    u8 unk4E[2];
    s16 unk50;
    s16 unk52;
    s32 unk54;
    struct Sub subs[12];
    u8 unk1A8;
    u8 unk1A9;
    u8 unk1AA;
    u8 unk1AB;
    u8 unk1AC;
    u8 unk1AD;
    u8 unk1AE;
    u8 unk1AF;
};

struct Entity {
    u8 unk00[8];
    s32 unk08;
    u8 unk0C[0xA];
    u8 unk16;
    u8 unk17[0x57];
    u16 unk6E;
    u8 unk70[0x3C];
    u8 *unkAC;
};

extern struct Block *gDomeEffectBlock[4] __asm__("D_800A22F0");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void *gBuildDomePatchMesh(s32 arg0, s32 arg1) __asm__("func_8006C84C");
extern void gResetMotionState(struct Sub *state) __asm__("func_8005CEA8");
struct Block *gAllocDomeEffectBlock(struct Entity *ent, s32 idx) __asm__("func_8005D3FC");


/**
 * @brief Allocates a 0x1B0-byte attached-effect block in D_800A22F0 with a dome mesh and 12 motion sub-states.
 *
 * Scans D_800A22F0[0..3] for the first free slot, allocates a 0x1B0-byte Block (tag 2),
 * attaches a dome-patch mesh via func_8006C84C (0x100/9 for ent->unk16 == 1 with colour pair
 * chosen by ent->unkAC[idx*2 + 0x8A] > 0, else 0x200/0xD), and initialises 12 Sub states.
 *
 * @param ent Owning Entity (+0x08 id, +0x16 kind, +0x6E scale, +0xAC state buffer).
 * @param idx Slot/weapon index stored at +0x54 and selecting the s16 check at ent->unkAC + idx*2 + 0x8A.
 * @return Newly allocated Block pointer stored in D_800A22F0, or NULL if all 4 slots are occupied.
 *
 * @par Globals
 * D_800A22F0 - 4-entry array of active Block pointers updated with the first free slot.
 * @par Control flow
 * Do-while loop scanning D_800A22F0[0..3] for NULL, branching on ent->unk16 == 1, then 12-iteration Sub init loop.
 * @par Calls
 * func_80017EBC - allocates 0x1B0-byte Block (tag 2); func_8006C84C - builds dome-patch mesh;
 * func_8005CEA8 - initialises each of the 12 0x1C-byte Sub motion blocks at Block + 0x58.
 * @par Side effects
 * Writes new Block pointer into D_800A22F0, allocates heap tag 2 and mesh buffers.
 * @par Callers
 * Confirmed: gWeaponEffectUpdate (lazy slot allocation), func_8006AB38 (kind-13 weapon slot setup).
 */
struct Block *gAllocDomeEffectBlock(struct Entity *ent, s32 idx)
{
    register struct Block **p __asm__("$16");
    struct Block **base;
    struct Block **q;
    s32 c7F;
    s32 c60;
    s32 c40;
    s32 i;

    c7F = 0x7F;
    c60 = 0x60;
    c40 = 0x40;

    base = gDomeEffectBlock;
    __asm__("" : "+r"(base));
    p = base;
    do {
        if (*p == 0) {
            *p = gAllocMemory(0x1B0, 2);

            if (ent->unk16 == 1) {
                if (*(s16 *)(ent->unkAC + (idx << 1) + 0x8A) > 0) {
                    (*p)->unk00 = gBuildDomePatchMesh(0x100, 9);
                    (*p)->unk1AB = c7F;
                    (*p)->unk1AC = c60;
                    (*p)->unk1AD = 0;
                    (*p)->unk1A8 = c40;
                    (*p)->unk1A9 = c40;
                    (*p)->unk1AA = c60;
                } else {
                    (*p)->unk00 = gBuildDomePatchMesh(0x100, 9);
                    (*p)->unk1AB = c60;
                    (*p)->unk1AC = 0x30;
                    (*p)->unk1AD = 0;
                    (*p)->unk1A8 = c40;
                    (*p)->unk1A9 = c40;
                    (*p)->unk1AA = c40;
                }
            } else {
                (*p)->unk00 = gBuildDomePatchMesh(0x200, 0xD);
                (*p)->unk1AB = c7F;
                (*p)->unk1AC = c7F;
                (*p)->unk1AD = c7F;
                (*p)->unk1A8 = c60;
                (*p)->unk1A9 = 0x30;
                (*p)->unk1AA = 0;
            }

            (*p)->unk04 = ent;
            (*p)->unk08 = ent->unk08;
            (*p)->unk52 = 0;
            (*p)->unk0C = 0;
            (*p)->unk4C = ent->unk6E * 2;
            (*p)->unk48 = 0;
            (*p)->unk4A = 0;
            (*p)->unk50 = 0;
            i = 0;
            (*p)->unk14 = 0;
            q = p;
            (*p)->unk54 = idx;
            (*p)->unk1AE = 0;

            for (; i < 12; i++) {
                gResetMotionState((struct Sub *)((u8 *)*q + 0x58 + i * 0x1C));
            }

            return *p;
        }
        p++;
    } while ((s32)p < (s32)(base + 4));

    return 0;
}
