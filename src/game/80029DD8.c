// Categories: Platform > Streaming Audio | Game Engine > Event Queue
#include "game/types.h"

extern u8 D_8009F69C;
extern u8 D_8009F6B8;
extern u8 D_8009F6C0;
extern s32 D_8009F6C4;

extern void func_80023E84(void);

/**
 * @brief Performs guarded stream/voice teardown and resets playback state flags.
 *
 * Executes once while D_8009F69C is 0: sets the guard D_8009F69C = 1, invokes func_80023E84 to stop active stream audio and voices, zeroes D_8009F6C4 and D_8009F6C0, and updates D_8009F6B8 by clearing bit 0x20 and setting bit 0x08.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F69C - teardown guard flag; D_8009F6B8 - stream/state bitfield; D_8009F6C0 - latch/counter cleared; D_8009F6C4 - timeout/timer word cleared.
 *
 * @par Control flow
 * Single guard check on D_8009F69C == 0; straight-line body inside.
 *
 * @par Calls
 * func_80023E84 - stops D_800A0A80 voices/handles and clears bit 0x10 of D_8009F6B8 via func_80029720.
 *
 * @par Side effects
 * Sets D_8009F69C = 1, clears D_8009F6C0/D_8009F6C4, modifies D_8009F6B8, and tears down active stream voices via func_80023E84.
 *
 * @par Callers
 * Confirmed: func_80025B7C, func_80029974, func_80029A60, func_80029EB8, func_8002A26C.
 */
void func_80029DD8(void)
{
    u8 flag;

    if (D_8009F69C == 0) {
        D_8009F69C = 1;
        func_80023E84();
        flag = D_8009F6B8;
        D_8009F6C4 = 0;
        D_8009F6C0 = 0;
        D_8009F6B8 = (flag & 0xDF) | 0x8;
    }
}
