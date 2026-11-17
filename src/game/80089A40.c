// Categories: Graphics > Video Decode Tables | Resources > Movie Playback
#include "game/types.h"

extern u32 D_8009F51C;
extern u32 D_8009F520;
extern u8 gDefaultSymbolTable[] __asm__("D_8009C560");
extern void func_80089A88(u32, u32, void *);

/**
 * @brief Builds the VLC decoding tables from the saved table blocks,
 *        with an optional symbol source.
 *
 * @param arg0 Symbol source (0x60 bytes); null selects the default
 *             D_8009C560.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80089A88(D_8009F51C, D_8009F520, arg0 or D_8009C560): the two
 * globals hold table blocks A and B, the callee builds the Huffman
 * lookup tables (block A's length table lives at +0x2000).
 *
 * @par Callers
 * func_800879DC.
 */
void func_80089A40(void *arg0)
{
    if (arg0 != 0) {
        func_80089A88(D_8009F51C, D_8009F520, arg0);
    } else {
        func_80089A88(D_8009F51C, D_8009F520, gDefaultSymbolTable);
    }
}
