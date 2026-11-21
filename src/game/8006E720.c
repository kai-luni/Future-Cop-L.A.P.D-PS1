// Categories: Graphics > Effects | Gameplay > Entity Script
#include "game/types.h"

struct UnkE720Sub {
    u8 pad[0x48];
    s16 unk48;
};

struct UnkE720 {
    u8 pad00[0x20];
    struct UnkE720Sub *unk20;
    u8 pad24[0x62];
    s16 unk86;
    u8 pad88;
    u8 unk89;
    u8 unk8A;
    u8 unk8B;
    u8 pad8C;
    u8 unk8D;
    u8 unk8E;
    u8 unk8F;
};
void gInitDomeEffectFadeRate(struct UnkE720 *self) __asm__("func_8006E720");


/**
 * @brief Computes a scaled duration/step value at +0x86 from peak effect colour channels and sub-record rate.
 *
 * Clears self->unk86 (+0x86) if below 0x10; otherwise takes the maximum across the six
 * intensity/colour bytes (+0x89..+0x8B and +0x8D..+0x8F) and scales it by
 * ((0x80000000u / (self->unk20->unk48 * 80 * 30)) >> 4) >> 4 into self->unk86.
 *
 * @param self Effect object holding sub-pointer at +0x20 (unk48 at +0x48), s16 counter at +0x86, and six byte channels at +0x89..+0x8F.
 *
 * @par Globals
 * None.
 * @par Control flow
 * Early exit zeroing unk86 when unk86 < 0x10, branch-based max across two 3-byte triplets, then fixed-point division and store.
 * @par Calls
 * None.
 * @par Side effects
 * Writes self->unk86 (+0x86).
 * @par Callers
 * Confirmed: func_8006E814 (message 0 effect init).
 */
void gInitDomeEffectFadeRate(struct UnkE720 *self)
{
    s32 a2;
    s32 v1;
    s32 t;
    s32 d;
    s32 h;
    s32 p;

    if (self->unk86 < 0x10) {
        self->unk86 = 0;
        return;
    }
    a2 = self->unk89 > self->unk8A ? self->unk89 : self->unk8A;
    if (self->unk8B > a2) {
        a2 = self->unk8B;
    }
    v1 = self->unk8D > self->unk8E ? self->unk8D : self->unk8E;
    if (self->unk8F > v1) {
        v1 = self->unk8F;
    }
    if (v1 > a2) {
        a2 = v1;
    }
    t = self->unk20->unk48;
    h = t << 4;
    p = t << 6;
    d = (p + h) * 30;
    self->unk86 = (s16)((((0x80000000u / (u32)d) >> 4) * a2) >> 4);
}
