// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    u8 unk00[4];
    s32 unk04;
    u8 unk08[4];
    s32 unk0C;
    u8 unk10[0x78];
    s32 unk88;
} Unk800A0968;

extern Unk800A0968 D_800A0968;
extern s32 D_8009FA2C;
extern void func_800226D8(void);
extern void func_8002A23C(void);

/**
 *  @brief Dispatches an operation based on the current state fields.
 *
 *  When the state is enabled, selects a follow-up operation using unk0C
 *  or compares unk88 against the global threshold.
 *  @par Globals
 *  D_800A0968 supplies the state fields and receives the unk88 reset.
 *  D_8009FA2C supplies the comparison threshold.
 *  @par Control flow
 *  Does nothing if unk04 is zero; otherwise branches on unk0C and the comparison.
 *  @par Calls
 *  func_800226D8 handles nonzero unk0C; func_8002A23C follows the reset.
 *  @par Side effects
 *  May set unk88 to -1 and invoke a function with additional effects.
 *  @par Callers
 *  Confirmed: func_8002441C.
 */
void func_80022654(void)
{
    if (D_800A0968.unk04 != 0) {
        if (D_800A0968.unk0C != 0) {
            func_800226D8();
        } else if (D_800A0968.unk88 < D_8009FA2C) {
            D_800A0968.unk88 = -1;
            func_8002A23C();
        }
    }
}
