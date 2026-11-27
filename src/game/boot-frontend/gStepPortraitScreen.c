// Categories: Game Engine > Object State
#include "game/types.h"

typedef struct {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x78];
    u8 unkA0;
    u8 padA1[3];
    s16 unkA4;
} Object;

extern s32 gFrameSwitch __asm__("D_8009FAC8");
extern s32 gPortraitStreamId __asm__("D_8009F9B4");
extern s32 gStreamPreemptFlag __asm__("D_8009F9B0");
void gStepPortraitScreen(Object *obj) __asm__("func_8004228C");


/**
 *  @brief Updates an object's tracked state against global activity.
 *
 *  For objects with unkA0 bit 0 set, this maintains bit 1 while activity is
 *  present or clears the tracked state when its active condition no longer holds.
 *  @param obj Object whose state and flags may be changed.
 *  @par Globals
 *  D_8009FAC8 supplies activity; D_8009F9B4 is compared with unkA4;
 *  D_8009F9B0 is decremented when the tracked state is cleared.
 *  @par Control flow
 *  If bit 1 is set and activity is absent or the identifier differs, clears
 *  state; otherwise, active objects have bit 1 set.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May set unkA4 to -10, clear unkA0 bits 0 and 1 and flags bit 3, or set bit 1.
 *  @par Callers
 *  Confirmed: func_80042330.
 */
void gStepPortraitScreen(Object *obj)
{
    s32 active;
    if (obj->unkA0 & 1) {
        active = gFrameSwitch;
        if ((obj->unkA0 & 2) && (!active || gPortraitStreamId != obj->unkA4)) {
            obj->unkA4 = -10;
            obj->unkA0 &= ~3;
            obj->flags &= ~8;
            gStreamPreemptFlag--;
        } else if (active) {
            obj->unkA0 |= 2;
        }
    }
}
