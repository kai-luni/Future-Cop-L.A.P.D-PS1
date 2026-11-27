// Categories: Graphics > GTE > Lighting | Graphics > Shading
#include "game/types.h"
void gGteShadeNccs(void *a0, void *a1, void *a2) __asm__("func_80015F14");


/**
 * @brief NCCS: shades a vertex normal/colour pair (normal colour, single vector).
 *
 * @param a0 Pointer to the transformed vertex (VXY0/VZ0 at +0/+4).
 * @param a1 Pointer to the source colour word (loaded into RGBC, register 6).
 * @param a2 Destination pointer for the result (RGB2, register 22).
 * @return Nothing; the interpolated colour is written to *a2.
 *
 * @par Confirmed mechanics
 * lwc2 $0/$1 (VXY0, VZ0), lwc2 $6 (RGBC), two nops, `cop2 0x0108041B` =
 * NCCS (real opcode 1Bh in bits 0-5, sf=1, lm=1), then swc2 $22 stores RGB2.
 * The command multiplies the light matrix by the vertex vector, adds the
 * background colour and multiplies the result by RGBC (the incoming colour).
 *
 * @par Callers
 * func_80015F38 and func_800172A8 - the two model-renderer paths that shade a
 * vertex before it is written to a packet.
 *
 * @par Environment path
 * Shades one vertex normal with the GTE NCCS command (light matrix,
 * background colour, then multiplied by the input colour) and stores the
 * resulting RGB2 word. Callers are gEmitShadedPart (func_80015F38) and
 * func_800172A8, the model renderer paths. It is model lighting, not level
 * construction or camera motion.
 */
void gGteShadeNccs(void *a0, void *a1, void *a2)
{
    __asm__ volatile(
        "lwc2 $0, 0($a0)\n\t"
        "lwc2 $1, 4($a0)\n\t"
        "lwc2 $6, 0($a1)\n\t"
        "nop\n\t"
        "nop\n\t"
        "cop2 0x0108041B\n\t"
        "swc2 $22, 0($a2)\n\t"
        :
        :
        : "memory"
    );
}
