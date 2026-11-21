// Categories: Graphics > Video Decoding
#include "game/types.h"

typedef struct {
    u8 iq_y[64];
    u8 iq_c[64];
    s16 dct[64];
} DecDctEnv;

extern s32 gMdecLuminanceQuantizationTable[] __asm__("D_8009E8B4");
extern s32 gMdecChrominanceQuantizationTable[] __asm__("D_8009E8F4");
extern s32 gMdecDefaultDctTable[] __asm__("D_8009E938");

/**
 *  @brief Initializes a decoder environment from three default tables.
 *
 *  Copies the luminance and chrominance quantization tables and the DCT
 *  table into the supplied environment.
 *  @param environment Destination decoder environment.
 *  @return The supplied environment pointer.
 *  @par Globals
 *  D_8009E8B4 and D_8009E8F4 supply quantization data; D_8009E938 supplies DCT data.
 *  @par Control flow
 *  Three loops copy 16, 16, and 32 words respectively.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the three tables in the destination environment.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
DecDctEnv *func_80095EC4(DecDctEnv *environment)
{
    s32 i;
    s32 *source;
    s32 *destination;

    destination = (s32 *)environment->iq_y;
    source = gMdecLuminanceQuantizationTable;
    for (i = 15; i != -1; i--) {
        *destination++ = *source++;
    }

    destination = (s32 *)environment->iq_c;
    source = gMdecChrominanceQuantizationTable;
    for (i = 15; i != -1; i--) {
        *destination++ = *source++;
    }

    destination = (s32 *)environment->dct;
    source = gMdecDefaultDctTable;
    for (i = 31; i != -1; i--) {
        *destination++ = *source++;
    }

    return environment;
}
