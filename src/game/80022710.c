// Categories: Resources > Streaming
#include "game/types.h"

struct StreamState {
    u8 flag;
    u8 unk01[0x0F];
    s16 unk10;
    u8 unk12[0x6E];
    s32 ready;
};

extern struct StreamState gMixerState __asm__("D_800A0968");
extern s32 gMixerWord[2] __asm__("D_800A0970");
extern u16 gStreamRequestSequence __asm__("D_8009F268");

extern void gResetAudioStreamController(void) __asm__("func_80022CA8");
extern void gSubmitAudioStreamTransfer(void) __asm__("func_80022D84");

s32 gPrimeAudioStream(void) __asm__("func_80022710");

/**
 * @brief Submits the first stream request and polls for readiness within a fixed iteration
 * budget.
 *
 * @param void No parameters.
 * @return The current value of D_800A0970[0].
 *
 * @par Confirmed mechanics
 * Resets the controller, increments the 16-bit sequence and substitutes 1 on wrap to zero.
 * Copies the sequence to mixer +0x10, calls gSubmitAudioStreamTransfer with a0 set to 1, then
 * polls mixer +0x80 with a 0xFFFF counter. If time remains it writes 1 to gMixerWord.
 *
 * @par Globals
 * Updates gMixerState, gStreamRequestSequence and gMixerWord.
 *
 * @par Callers
 * Direct calls from gInitializeVagmAudioStream (func_800244A4).
 *
 * @par Role and path
 * Initial stream setup uses the returned word before starting configured channels.
 *
 * @par Unknown
 * The spin budget counts iterations and the timeout path returns the existing result word.
 */
s32 gPrimeAudioStream(void)
{
    s32 i;
    s32 j;
    struct StreamState *p;
    struct StreamState *q;

    i = 0xFFFF;
    gResetAudioStreamController();
    p = &gMixerState;
    p->flag = 1;
    gStreamRequestSequence++;
    if (gStreamRequestSequence == 0) {
        gStreamRequestSequence = 1;
    }
    p->unk10 = gStreamRequestSequence;
    gSubmitAudioStreamTransfer();

    if (p->ready == 0) {
        q = p;

        do {
            j = i;
            i--;
        } while (j != 0 && q->ready == 0);
    }
    if (i != 0) {
        gMixerWord[0] = 1;
    }
    return gMixerWord[0];
}
