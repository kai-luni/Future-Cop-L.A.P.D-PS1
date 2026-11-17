// Categories: Gameplay > Mode State
#include "game/types.h"

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Mode;

extern Mode *gGameSession __asm__("D_8009FA94");
extern u8 D_8009FAC4;
extern u8 D_800A35FD[];
extern u8 D_800A35FB[];
extern u8 gOctalDigitTable[] __asm__("D_8009C1BC");
extern u8 gHexDigitTable[] __asm__("D_8009C1AC");
extern u8 gPasswordBuffer[] __asm__("D_800A3948");
extern u8 D_800A3938[];
extern volatile s32 D_8009F8F8;
extern u16 D_8009F92A;
extern void func_8007F02C(void);

/**
 *  @brief Initializes mode-dependent state and generates ten table-derived values.
 *
 *  Alternating lookup tables transform source bytes into zero-based letter values.
 *  Two mode fields also select flags and a final state byte.
 *  @par Globals
 *  D_8009FA94 supplies mode fields; D_8009FAC4 and D_800A35FD are set.
 *  D_8009C1BC and D_8009C1AC are lookup tables; D_800A3938 is the source.
 *  D_800A3948 receives results; D_8009F8F8 and D_8009F92A are cleared.
 *  D_800A35FB receives the mode-dependent state byte.
 *  @par Control flow
 *  Selects an initial flag, loops over ten bytes, then branches on mode fields.
 *  @par Calls
 *  func_8007F02C runs before the table transformation.
 *  @par Side effects
 *  Updates the listed global flags, counters, output bytes and state byte.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8007E7A8(void)
{
    s32 i;
    u8 *odd;
    u8 *even;
    u8 *dst;
    u8 *src;
    u8 *letter;
    Mode *p;

    D_8009FAC4 = 1;
    if (gGameSession->unk0C == 2) {
        D_800A35FD[0] = 0;
    } else {
        D_800A35FD[0] = 1;
    }
    func_8007F02C();
    i = 0;
    odd = gOctalDigitTable;
    even = gHexDigitTable;
    dst = gPasswordBuffer;
    src = D_800A3938;
    do {
        if (i & 1) {
            letter = (u8 *)(((*src + i) & 7) + (u32)odd);
        } else {
            letter = (u8 *)(((*src + i) & 0xF) + (u32)even);
        }
        *dst++ = *letter - 0x41;
        __asm__ volatile("");
        i++;
        src++;
    } while (i < 10);
    p = gGameSession;
    D_8009F8F8 = 0;
    __asm__ volatile("" ::: "memory");
    D_8009F92A = 0;
    if (p->unk5C == 1) {
        if (p->unk0C == 1) {
            D_800A35FB[0] = 9;
        } else {
            D_800A35FB[0] = 3;
        }
    } else if (p->unk0C == 1) {
        D_800A35FB[0] = 1;
    } else {
        D_800A35FB[0] = 8;
    }
}
