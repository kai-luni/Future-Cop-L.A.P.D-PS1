// Categories: Platform > Sound | Platform > Streaming Audio
#include "game/types.h"

typedef struct {
    u8 pad00[8];
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x10];
    u8 unk20;
    u8 pad21[0x23];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
} Unk800A0B70;

extern Unk800A0B70 gStreamBufferList __asm__("D_800A0B70");
extern u8 gBitFieldState __asm__("D_8009F6B8");
extern s32 gCallGateFlag __asm__("D_8009F6DC");
extern s32 gVagmPrimeBlockCount __asm__("D_8009F6E0");
extern s32 D_8009F6E4;

extern s32 func_80045288(void);
void gStartVagmAudioStream(s32 arg0) __asm__("func_8002A16C");


/**
 * @brief Triggers stream playback state transition in D_800A0B70 when audio is active.
 *
 * Queries volume via func_80045288 when low byte of arg0 is non-zero; if D_800A0B70 is
 * armed (unk44 != 0, unk20 != 0), volume > 0, and !(D_8009F6B8 & 0x40), advances D_8009F6DC and sets playback offset.
 *
 * @param arg0 Non-zero low byte enables the func_80045288 volume check; 0 assumes value = 1.
 *
 * @par Globals
 * D_800A0B70 (stream state: unk08/0C/20/44/48/4C), D_8009F6B8 (flag 0x40 gate), D_8009F6DC, D_8009F6E0, D_8009F6E4.
 * @par Control flow
 * Branches on (arg0 & 0xFF) for value, then checks four guard conditions before updating D_8009F6DC (3 or 4) and D_800A0B70 fields.
 * @par Calls
 * func_80045288 - returns D_8009F334 volume setting when (arg0 & 0xFF) != 0.
 * @par Side effects
 * Updates D_8009F6DC, D_8009F6E0 (when starting from 0), and D_800A0B70.unk0C/unk20/unk08.
 * @par Callers
 * Confirmed: D_8007D18C, func_8002902C, func_8002A26C, func_80047690, func_80080984; func_8002A23C (at 0x8002A254 in asm/us/main.s).
 */
void gStartVagmAudioStream(s32 arg0)
{
    Unk800A0B70 *p;
    s32 value;

    if ((arg0 & 0xFF) != 0) {
        value = func_80045288();
    } else {
        value = 1;
    }

    p = &gStreamBufferList;

    if (p->unk44 != 0 && value > 0 && p->unk20 != 0 &&
        !(gBitFieldState & 0x40)) {
        if (gCallGateFlag == 0) {
            gCallGateFlag = 3;
            gVagmPrimeBlockCount = 0;
        } else {
            gCallGateFlag = 4;
        }

        p->unk0C = -1;
        p->unk20 = 0;
        p->unk08 = p->unk4C + p->unk48 + D_8009F6E4 * 0x6000;
    }
}
