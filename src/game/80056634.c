// Categories: Graphics > Camera | Gameplay > Player
#include "game/types.h"

typedef struct Vec56634 {
    s32 x;
    s32 y;
    s32 z;
} Vec56634;

typedef struct Entry56634 {
    s16 f0;
    s16 f2;
    s32 f4;
    Vec56634 pos;
} Entry56634;

typedef struct Info56634 {
    u8 unk00[0xA6];
    u8 flagA6;
    u8 flagA7;
    s16 valA8;
    s16 valAA;
    s8 idAC;
    u8 unkAD[0x0F];
    s32 valBC;
} Info56634;

typedef struct Object56634 {
    u8 unk00[0xAC];
    Info56634 *info;
} Object56634;

typedef struct Player56634 {
    u8 unk00[0x50];
    Vec56634 pos;
    u8 unk5C[0x92];
    s16 angle;
    u8 unkF0[0x2E];
    s8 index;
} Player56634;

extern Player56634 *gMissionWorkingSet __asm__("D_8009F9A0");
extern Entry56634 gFormCodeTable[] __asm__("D_8009ACF4");
extern Object56634 *D_8009FA58[2];

extern void func_8005612C(Vec56634 *pos, s32 angle, s32 angle2, s32 pitch, s32 dist,
                          Vec56634 *src, Vec56634 *dst, s32 val, s32 val2);
extern void func_80017B60(s32 id);
extern void func_800177AC(Vec56634 *position);
extern void func_80017860(s32 yaw, s32 pitch, s32 roll);

/**
 * @brief Places both players' view objects using the per-view offset table.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Reads the active view index and angle from D_8009F9A0 (+0x11E index,
 * +0xEE angle), selects the entry D_8009ACF4[index] {f0, f2, f4, pos}. For
 * each non-null player D_8009FA58[i]: func_8005612C computes the world
 * position from the view position/angle plus the entry offsets and the
 * player's info valA8/valBC; func_80017B60 selects the draw id (info
 * +0xAC), func_800177AC sets the position, func_80017860 the rotation
 * (angle, entry f2, 0); info flags +0xA6/+0xA7 become 1 and valAA is
 * cleared.
 *
 * @par Likely role
 * The per-frame camera-anchor placement of the screen frame func_800259D8:
 * each player carries a view object whose offset/tilt comes from the
 * per-view table, so split-screen views keep their relative placement.
 *
 * @par Unknown
 * The meaning of the two info values valA8/valBC passed to the placement.
 *
 * @par Callers
 * func_800259D8.
 */
void func_80056634(void)
{
    s32 i;
    s32 angle;
    Entry56634 *entry;
    Vec56634 out;

    register Entry56634 *entries __asm__("$2");
    s32 index;

    index = gMissionWorkingSet->index;
    __asm__("" : : "r"(index));
    angle = gMissionWorkingSet->angle;
    entries = gFormCodeTable;
    entry = &entries[index];

    for (i = 0; i < 2; i++) {
        if (D_8009FA58[i] == 0) {
            continue;
        }

        func_8005612C(&gMissionWorkingSet->pos, angle, angle + entry->f0, entry->f2,
                      entry->f4, &entry->pos, &out,
                      D_8009FA58[i]->info->valA8, D_8009FA58[i]->info->valBC);

        func_80017B60(D_8009FA58[i]->info->idAC);
        func_800177AC(&out);
        func_80017860(angle, entry->f2, 0);

        {
            Info56634 *info = D_8009FA58[i]->info;
            info->flagA7 = 1;
            info->flagA6 = 1;
        }
        D_8009FA58[i]->info->valAA = 0;
    }
}
