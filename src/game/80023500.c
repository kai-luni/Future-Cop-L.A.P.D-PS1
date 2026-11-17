// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern u8 D_8009F26C;
extern s32 D_8009F278;
extern s32 D_8009F27C;
extern u8 D_80023314[];
extern Unk800A0A00 gObjectSlots[8] __asm__("D_800A0A00");

extern void func_80098708(s32 arg0);
extern s32 func_8001F12C(s32 callback);
extern void func_80021250(void);
extern void func_80023BF8(void);
extern void func_80021C80(void);
extern void func_80044FEC(void);

/**
 *  @brief Sets the status byte of an indexed entry to 2.
 *
 *  Updates the unk02 field in an eight-entry global table.
 *  Indices outside the table are ignored.
 *  @param arg0 Unsigned index of the entry to update.
 *  @par Globals
 *  D_800A0A00 is the table whose selected unk02 field is written.
 *  @par Control flow
 *  Checks that arg0 is less than 8 before writing.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May change one table entry's status byte.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80023500(u32 arg0)
{
    if (arg0 < 8) {
        gObjectSlots[arg0].unk02 = 2;
    }
}
