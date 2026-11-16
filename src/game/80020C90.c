// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

struct CardManager {
    s32 unk00;
    s32 state[16];
    void *primary[16];
};

extern struct CardManager *D_8009AB08;

extern s32 func_80020A9C(s32 a0);
extern void func_800207CC(void);
extern s32 func_8008CCA0(void *arg0, void *arg1);

/**
 * @brief Searches active memory card slots for a save file matching the given name.
 *
 * Verifies card presence via func_80020A9C(1), refreshes the directory table in D_8009AB08
 * via func_800207CC, and compares arg0 against primary names across slots 0..15 with state == 1.
 *
 * @param arg0 Null-terminated save filename string to match against primary slot names.
 * @return Matching slot index (1..15, or 0 if slot 0 matched), or 0 if card check fails or no slot matches.
 *
 * @par Globals
 * D_8009AB08 - CardManager pointer whose state[0..15] and primary[0..15] arrays are scanned.
 * @par Control flow
 * Guards on func_80020A9C(1) != 0; refreshes directory, then loops i = 0..15 breaking on first state[i]==1 and strcmp==0.
 * @par Calls
 * func_80020A9C (card presence/status check), func_800207CC (directory parse), func_8008CCA0 (string compare).
 * @par Side effects
 * Updates D_8009AB08 directory state via func_800207CC when card is ready.
 * @par Callers
 * Confirmed: func_80020D3C (read save slot), func_80020FDC (write/update save slot), func_8007FB04 (front-end card probe).
 */
s32 func_80020C90(void *arg0)
{
    register s32 ret asm("s1") = 0;
    register u32 i asm("s0");
    register s32 one asm("s2");

    if (func_80020A9C(1) != 0) {
        func_800207CC();
        __asm__("" : "=r"(ret) : "0"(ret));
        i = ret;
        one = 1;
        do {
            if (D_8009AB08->state[i] == one) {
                if (func_8008CCA0(arg0, D_8009AB08->primary[i]) == 0) {
                    ret = i;
                    break;
                }
            }
            i++;
        } while (i < 16);
    }
    return ret;
}
