// Categories: Game Engine > State Machine | Gameplay > Entity Script | Utilities > Random
#include "game/types.h"

struct Unk69B58 {
    u8 unk00[0x18];
    u32 unk18;
    u32 unk1C;
    u32 unk20;
    u32 unk24;
};

extern struct Unk69B58 *gGameSession __asm__("D_8009FA94");
extern s32 *gOpcodeTable __asm__("D_8009F9F8");
extern s32 *gScriptVmVars __asm__("D_8009FAD4");
extern s32 *gVmCountdowns __asm__("D_8009F9A4");
extern u8 gScriptVmDisabled __asm__("D_8009F840");

extern s32 func_80019668(void);
extern void gBindGameSessionPointers(void) __asm__("func_80069B1C");
void gInitScriptVmState(void) __asm__("func_80069B58");


/**
 * @brief Initializes script-VM random tables, variable banks, and state pointers, and enables bytecode script execution.
 *
 * Confirmed: fills D_8009FA94->unk18..unk24 (4 words) and D_8009F9F8[0..31] (32 words) with func_80019668() % 1000, zeroes D_8009FAD4[0..99], sets D_8009F9A4[0..31] to -10, invokes func_80069B1C() to populate D_8009FA94->unkE4..unkF0, and clears D_8009F840 = 0. Category follows script VM globals (D_8009F840 inhibit byte, D_8009F9F8/D_8009FAD4/D_8009F9A4 banks used in func_80069D68) and func_80019668 RNG.
 * Likely: per-mission script-VM reset before entities spawn; -10 in D_8009F9A4 represents -2 after func_80069D68's divide-by-5 scaling. Unknown: specific role of D_8009FA94->unk18..unk24 vs D_8009F9F8.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA94 (+0x18..+0x24 written), D_8009F9F8 (32-word table written), D_8009FAD4 (100-word bank zeroed), D_8009F9A4 (32-word bank set to -10), D_8009F840 (cleared to 0).
 * @par Control flow
 * Four unnested counted loops (4 unrolled assignments, 32 forward, 100 downward, 32 downward), then func_80069B1C and byte store.
 * @par Calls
 * func_80019668 - supplies random numbers modulo 1000. func_80069B1C - stores D_8009FAA4/FAB4/FA70/FACC addresses into D_8009FA94->unkE4..unkF0.
 * @par Side effects
 * Overwrites global script variable banks, random table, D_8009FA94 fields, and clears D_8009F840.
 * @par Callers
 * Confirmed: func_80024904 (0x80024974) and func_80024A24 (0x80024C08), during mission/level initialization / Likely: none / Unknown: none.
 */
void gInitScriptVmState(void)
{
    s32 i;
    s32 value;

    gGameSession->unk18 = (u32)func_80019668() % 1000;
    gGameSession->unk1C = (u32)func_80019668() % 1000;
    gGameSession->unk20 = (u32)func_80019668() % 1000;
    gGameSession->unk24 = (u32)func_80019668() % 1000;

    for (i = 0; i < 0x20; i++) {
        gOpcodeTable[i] = (u32)func_80019668() % 1000;
    }

    for (i = 0x63; i >= 0; i--) {
        gScriptVmVars[i] = 0;
    }

    value = -10;
    for (i = 0x1F; i >= 0; i--) {
        gVmCountdowns[i] = value;
    }

    gBindGameSessionPointers();
    gScriptVmDisabled = 0;
}
