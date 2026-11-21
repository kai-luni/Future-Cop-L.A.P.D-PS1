// Categories: Utilities > Fixed-point Math
#include "game/types.h"

typedef struct {
    u8 pad00[0x66];
    u16 flags66;
    u8 pad68[0x1F];
    u8 b87;
    u8 b88;
    u8 b89;
    u8 pad8A;
    u8 b8B;
    u8 b8C;
    u8 b8D;
    s16 out8E;
    s16 out90;
    s16 out92;
    s16 div94;
} Unk800403D0;
void gInitObjectLightColorSteps(Unk800403D0 *p) __asm__("func_800403D0");


/**
 *  @brief Computes three fixed-point output values from fields in an object.
 *
 *  Flag 0x100 selects a shared value derived from the sum of three bytes.
 *  Otherwise each output uses the difference between corresponding byte fields.
 *  @param p Object containing the flag, input bytes, divisor and output fields.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  A divisor below 16 zeros all outputs; otherwise the selected numerators are shifted by 12 and divided.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the three signed 16-bit output fields in p.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gInitObjectLightColorSteps(Unk800403D0 *p)
{
    if (p->flags66 & 0x100) {
        if (p->div94 < 0x10) {
            p->out8E = 0;
            p->out90 = 0;
            p->out92 = 0;
        } else {
            s32 v = ((p->b87 + p->b88 + p->b89) << 12) / p->div94;
            p->out8E = v;
            p->out90 = v;
            p->out92 = v;
        }
    } else {
        if (p->div94 < 0x10) {
            p->out8E = 0;
            p->out90 = 0;
            p->out92 = 0;
        } else {
            p->out8E = ((p->b8B - p->b87) << 12) / p->div94;
            p->out90 = ((p->b8C - p->b88) << 12) / p->div94;
            p->out92 = ((p->b8D - p->b89) << 12) / p->div94;
        }
    }
}
