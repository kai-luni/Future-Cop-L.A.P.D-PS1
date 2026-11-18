// Categories: Graphics > Decoder Environment
#include "game/types.h"

typedef struct {
    u8 iq_y[64];
    u8 iq_c[64];
    s16 dct[64];
} DecDctEnv;

extern s32 D_8009E8B0[];
extern s32 D_8009E8B4[];
extern s32 D_8009E8F4[];
extern s32 D_8009E934[];
extern void gStartMdecBlockTransfer(s32 *data, u32 size) __asm__("func_80096240");

/**
 *  @brief Copies decoder quantization tables and submits two data blocks.
 *
 *  Copies the environment's luminance and chrominance tables into global
 *  storage, then passes two global blocks to the decoder helper.
 *  @param environment Source decoder environment containing the tables.
 *  @return The supplied environment pointer.
 *  @par Globals
 *  D_8009E8B4 and D_8009E8F4 receive the tables; D_8009E8B0 and
 *  D_8009E934 are passed to the helper.
 *  @par Control flow
 *  Two loops copy 16 words each, followed by two helper calls.
 *  @par Calls
 *  gStartMdecBlockTransfer receives each global block with size 0x20.
 *  @par Side effects
 *  Overwrites the two global table regions; the helper may have further effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
DecDctEnv *func_80095F50(DecDctEnv *environment)
{
    s32 i;
    s32 *source;
    s32 *destination;

    destination = D_8009E8B4;
    source = (s32 *)environment->iq_y;
    for (i = 15; i != -1; i--) {
        *destination++ = *source++;
    }

    destination = D_8009E8F4;
    source = (s32 *)environment->iq_c;
    for (i = 15; i != -1; i--) {
        *destination++ = *source++;
    }

    gStartMdecBlockTransfer(D_8009E8B0, 0x20);
    gStartMdecBlockTransfer(D_8009E934, 0x20);

    return environment;
}
