// Categories: Gameplay > Entity Update | Game Engine > Entity Lifecycle | Physics > Collision
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C;
    u8 unk0D[0x09];
    u8 unk16;
    u8 unk17[0x11];
    u8 unk28[0x04];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    struct Vec3 unk50;
    struct Vec3 unk5C;
    u8 unk68[0x38];
    u8 unkA0;
    u8 unkA1;
    s16 unkA2;
    s32 unkA4;
    struct Entity *unkA8;
    s32 unkAC;
    s8 unkB0;
    s8 unkB1;
};

extern struct Entity *func_8004D69C(s32 tag);
extern void func_80017668(void *instance, s32 part, s32 *out);
extern s32 func_80030DE0(struct Entity *self, void *data, s32 arg);
extern u8 func_8002F8B8(u8 arg0, s16 arg1, void *arg2, s32 arg3);
extern s32 func_80031238(struct Entity *self, void *data);
extern s32 func_80031120(struct Entity *self, void *data, s32 arg);
extern s32 func_8002AD58(s32 message, struct Entity *self, void *data);
extern void func_80031004(struct Entity *self);
extern s32 D_8009FAA4;
extern void *jtbl_8001123C[];

/**
 * @brief Message handler for trigger/attachment entities (types 14 and 38), dispatching messages 1..13 via jtbl_8001123C.
 *
 * Confirmed: routes message 1..13 through jtbl_8001123C and defaults others to func_8002AD58. Case 1 returns 0xB8 (size); case 2 calls func_80030DE0; case 11 calls func_80031120. Case 6 runs func_80031004 when self->unk0C != 0 and func_800313F0 when self->unk16 == 0x26. Case 10 returns 0 if self->unkA0 & 0x10, else evaluates func_8002F8B8, filters type 0x26 with func_80031238, and suppresses duplicate hits when self->unkA4 == D_8009FAA4 and self->unkA0 & 4. Category follows jtbl_8001123C dispatch, 0xB8 size, func_80031238 collision, and func_800313F0 sync. Likely: reached indirectly via entity update pointers. Unknown: distinction between types 14 and 38.
 *
 * @param message Message selector (1=size, 2=init, 6=tick, 10=overlap/match query, 11=interact, others fallback).
 * @param arg1 Entity block (+0x0C commands, +0x16 kind, +0xA0..+0xB1 trigger/attachment state).
 * @param arg2 Message payload or candidate entity for cases 2/10/11.
 * @param arg3 Extra argument forwarded to case 2 and 11 helpers.
 * @return Case 1: 0xB8; case 10: 0 or 1 match flag; cases 3-5/7-9/12-13: 0; others return helper result.
 * @par Globals
 * D_8009FAA4 - per-frame stamp tested against self->unkA4 in case 10. jtbl_8001123C - 13-entry jump table.
 * @par Control flow
 * Bounds check (u32)(message - 1) >= 13 jumps to fallback; otherwise switches via jtbl_8001123C.
 * @par Calls
 * func_80030DE0 (case 2 init), func_8002F8B8 and func_80031238 (case 10 query/OBB), func_80031120 (case 11 attach/flags), func_80031004 and func_800313F0 (case 6), func_8002AD58 (default).
 * @par Side effects
 * Case 6/11 mutate entity flags, commands, attachment transform, or linked entities through sub-helpers.
 * @par Callers
 * Confirmed: none direct; func_8002AE54 returns D_8003151C (alias of func_8003151C) at 0x8002AF84 for types 14 and 38 / Likely: entity update callers via function pointer / Unknown: specific indirect call sites.
 */
s32 func_8003151C(s32 message, struct Entity *arg1, void *arg2, s32 arg3)
{
    register struct Entity *self __asm__("$16");
    register void *data __asm__("$17");
    register s32 a3v __asm__("$7");
    u32 index;
    void **table;
    s32 result;
    static void *const label_table[] __attribute__((used)) = {
        &&m1, &&m2, &&mzero, &&mzero, &&mzero, &&m6, &&mzero, &&mzero,
        &&mzero, &&m10, &&m11, &&mzero, &&mzero
    };

    a3v = arg3;
    self = arg1;
    data = arg2;
    __asm__("" : "=r"(self), "=r"(data) : "0"(self), "1"(data));
    index = (u32)message - 1;
    if (index >= 13) {
        goto fallback;
    }
    table = jtbl_8001123C;
    __asm__("" : "=r"(table) : "0"(table));
    goto *table[index];

m1:
    return 0xB8;
m2:
    return func_80030DE0(self, data, a3v);
m10:
    a3v = self->unkA0;
    if (a3v & 0x10) {
        return 0;
    }
    result = func_8002F8B8(self->unkA1, self->unkA2, data, ((u32)a3v >> 2) & 1);
    if (result != 0) {
        if (self->unk16 == 0x26) {
            result = func_80031238(self, data);
        }
        if (result != 0) {
            if (self->unkA4 == D_8009FAA4 && (self->unkA0 & 4)) {
                result = 0;
            }
        }
    }
    return result;
m11:
    return func_80031120(self, data, a3v);
m6:
    if (self->unk0C != 0) {
        func_80031004(self);
    }
    if (self->unk16 == 0x26) {
        func_800313F0(self);
    }
mzero:
    return 0;
fallback:
    return func_8002AD58(message, self, data);
}
