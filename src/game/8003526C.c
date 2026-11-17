// Categories: Gameplay > Entity Update | AI > Behaviour
#include "game/types.h"

struct State {
    u8 pad00[0x18];
    s16 unk18;
    u8 pad1A[4];
    s16 unk1E;
    u16 unk20;
    u8 pad22[2];
    s32 unk24;
};

struct Entity {
    u8 pad00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    u8 pad28[0xCC];
    s32 unkF4;
    s16 unkF8;
    u8 unkFA;
    u8 padFB[0x39];
    struct State *unk134;
};

struct GlobalFA94 {
    u8 pad00[0xE0];
    s32 unkE0;
};

extern struct GlobalFA94 *gGameSession __asm__("D_8009FA94");

extern void func_8003BA50(struct Entity *self, s32 old1C);
extern s32 func_80019668(void);
extern void func_800330E8(struct Entity *self);

/**
 * @brief Resets a drone entity's counter, AI state, and strategic target position.
 *
 * Restores self->unk1C from unk1E, optionally triggers quartile-change flagging,
 * randomizes the AI record threshold, clears state unkFA, updates unkDC via func_800330E8,
 * copies speeds from unk134, and writes 4 to D_8009FA94->unkE0.
 *
 * @param self Entity block (+0x1C/+0x1E counters, +0x20/+0x24 flags, +0xFA state, +0x134 AI state).
 *
 * @par Globals
 * D_8009FA94 - global game/AI state pointer; field +0xE0 is set to 4.
 * @par Control flow
 * Saves old unk1C, sets unk1C = unk1E, conditionally calls func_8003BA50 when unk20 != 0 and !(unk24 & 0x40000), then updates AI and speed fields sequentially.
 * @par Calls
 * func_8003BA50 (quartile flag update), func_80019668 (random bits & 0x3F for AI threshold), func_800330E8 (computes strategic target self->unkDC).
 * @par Side effects
 * Mutates self->unk1C, unkFA, unkF4, unkF8, self->unk134->unk1E/unk18, and D_8009FA94->unkE0.
 * @par Callers
 * Confirmed: func_80030060 (contact reaction on flag 0x20000), func_8003532C (drone tick reset).
 */
void func_8003526C(struct Entity *self)
{
    s32 old1C = self->unk1C;
    self->unk1C = self->unk1E;
    if (self->unk20 != 0 && !(self->unk24 & 0x40000)) {
        func_8003BA50(self, old1C);
    }
    self->unk134->unk1E = ((s16)self->unk1E >> 1) - (func_80019668() & 0x3F);
    self->unkFA = 0;
    func_800330E8(self);
    self->unk134->unk18 = 0;
    self->unkF4 = self->unk134->unk24;
    self->unkF8 = self->unk134->unk20;
    gGameSession->unkE0 = 4;
}
