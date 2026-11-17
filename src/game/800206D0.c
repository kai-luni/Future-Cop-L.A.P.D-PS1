// Categories: Resources > Movie Playback | Platform > MDEC
#include "game/types.h"

struct Ctx {
    s32 unk000;
    u8 unk004[0x8F0];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    s32 unk908;
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    s32 unk9A0;
    s32 unk9A4;
    s32 unk9A8;
    s32 unk9AC;
    u8 unk9B0[0x1200];
    s32 unk1BB0;
};

extern volatile struct Ctx *gOperationState __asm__("D_8009AB08");

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_80020530(void);
extern void func_80020120(void);
extern void func_8008D640(s32 value);

/**
 * @brief Resets the movie decoder context to its start-of-stream state.
 *
 * @return Nothing.
 *
 * @par Confirmed field values written
 * unk8F4 = unk8F8 = unk910 = 0, unk8FC = the context's first word (the channel
 * value set by func_80020618), unk900 = -2, unk904 = 0, unk908 = 1, unk90C = 0,
 * unk994 = unk998 = 0, unk9A0 = 3, unk9AC = 3, unk9A8 = 1, unk9A4 = 0,
 * unk1BB0 = 0; then the 0x80-byte area at unk914 and the 0x1200-byte area at
 * unk9B0 are cleared, func_80020530() and func_80020120() re-arm the two
 * sub-blocks and func_8008D640(unk8FC) hands the channel back to the driver.
 *
 * @par Globals
 * D_8009AB08 (decoder context).
 *
 * @par Likely role
 * "rewind to frame 0" before a new movie is armed; func_8007FA88 is what calls
 * it, right before it sets the player state to 6 and requests playback.
 *
 * @par Callers
 * func_8007FA88 only.
 */
void func_800206D0(void)
{
    s32 t = gOperationState->unk000;

    gOperationState->unk8F4 = 0;
    gOperationState->unk8F8 = 0;
    gOperationState->unk910 = 0;
    gOperationState->unk8FC = t;
    gOperationState->unk900 = -2;
    gOperationState->unk904 = 0;
    gOperationState->unk908 = 1;
    gOperationState->unk90C = 0;
    gOperationState->unk994 = 0;
    gOperationState->unk998 = 0;
    gOperationState->unk9A0 = 3;
    gOperationState->unk9AC = 3;
    gOperationState->unk9A8 = 1;
    gOperationState->unk9A4 = 0;
    gOperationState->unk1BB0 = 0;

    func_8001E4C0(gOperationState->unk914, 0, 0x80);
    func_8001E4C0(gOperationState->unk9B0, 0, 0x1200);
    func_80020530();
    func_80020120();
    func_8008D640(gOperationState->unk8FC);
}
