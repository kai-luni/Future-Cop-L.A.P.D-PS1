// Categories: Platform > Transfer State
#include "game/types.h"

typedef struct {
    volatile s32 unk00;
    u8 pad04[0x7B44];
    volatile s32 unk7B48;
    volatile s32 unk7B4C;
    u8 *unk7B50;
    u32 unk7B54;
} State;
extern State *gMovieAudioState __asm__("D_8009F4D8");
extern void gUploadMovieAudioBlock(u8 *, u32, u32) __asm__("func_8008684C");
void gStepMovieAudioSecondChannelTransfer(s32 arg0, State *arg1) __asm__("func_80086954");


/**
 *  @brief Advances a two-stage operation in a state structure.
 *
 *  State 1 submits the stored buffer and length, while state 2 clears the
 *  state and optionally advances a counter by 0x300.
 *  @param arg0 Unused argument.
 *  @param arg1 State containing the operation status, buffer, and counter.
 *  @par Globals
 *  gMovieAudioState points to the state whose status is set to 2 after submission.
 *  @par Control flow
 *  Branches on status 1 or 2; other status values cause no action.
 *  @par Calls
 *  func_8008684C receives the stored buffer, length, and constant 1.
 *  @par Side effects
 *  Updates status fields and may increment unk00 and clear unk7B4C.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gStepMovieAudioSecondChannelTransfer(s32 arg0, State *arg1)
{
    if (arg1->unk7B48 == 1) {
        gUploadMovieAudioBlock(arg1->unk7B50, arg1->unk7B54, 1);
        gMovieAudioState->unk7B48 = 2;
    } else if (arg1->unk7B48 == 2) {
        arg1->unk7B48 = 0;
        if (arg1->unk7B4C != 0) {
            arg1->unk00 += 0x300;
            arg1->unk7B4C = 0;
        }
    }
}
