// Categories: Graphics > 2D Drawing > Lines | Graphics > HUD
#include "game/types.h"

struct LineF3 {
    u32 tag;
    u32 code;
    s16 xy[6];
};

extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");
extern s16 gRadiusTable[] __asm__("D_8009AFB4");

extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void gEmitRadarTriangle(s32 origin, u32 angle, s32 code) __asm__("func_8005A828");


/**
 * @brief Emits one flat-shaded triangle (POLY_F3, code 0x20) whose three
 *        corners come from a shape table rotated around a radar origin.
 *
 * @param origin X origin of the shape in 16.16 fixed point (0x780000 = 120 px
 *        for the one radar side, 0x280000 = 40 px for the other).
 * @param angle Rotation angle in the 4096-step convention.
 * @param code Colour word for the primitive; the base opcode 0x20 is OR-ed in.
 * @return Nothing; one 0x1C-byte packet from D_8009FA90, guarded by D_8009F9C0.
 *
 * @par Confirmed
 * func_8001846C supplies sine and cosine; a single loop runs ctr = 0..2 and
 * consumes three point pairs from the half-word table D_8009AFB4, writing them
 * as the three vertices of one primitive with tag 0x04000000 and the command
 * word `code | 0x20000000`, i.e. a POLY_F3, not three separate lines.
 *
 * @par Likely role
 * One radar marker: the node's shape table rotated by its heading, drawn at the
 * radar origin of the player side.
 *
 * @par Environment path
 * Takes one 28-byte packet from the gPacketCursor pool (not advancing when
 * gPacketLimit is reached) with tag word 0x04000000 (length 4, link 0) and
 * command word code or 0x20000000, i.e. a POLY_F3 with the colour passed by
 * the caller. The three vertices are the three point pairs of
 * gRadiusTable rotated by the sine and cosine from func_8001846C, with
 * x offset by the 16.16 origin and y offset by 0x2C0000 (44 px), then shifted
 * right 16. Called only by func_8005A9C4 with a gMarkerColorTable colour.
 */
void gEmitRadarTriangle(s32 origin, u32 angle, s32 code)
{
    struct LineF3 *line;
    s32 values[2];
#define sine values[0]
#define cosine values[1]
    register s32 s __asm__("$6");
    register s32 c __asm__("$7");
    s32 i;
    register s32 bias __asm__("$9");
    s16 *out;
    register s16 *points __asm__("$4");
    register s32 ctr __asm__("$8");
    register s32 *arg1 __asm__("$5");
    register s32 *arg2 __asm__("$6");
    register u32 arg0 __asm__("$4");
    s32 saved_code;

    saved_code = code;
    arg0 = angle;
    arg1 = &sine;
    __asm__ volatile ("" : "+r"(arg0), "+r"(arg1), "+r"(saved_code));
    arg2 = &cosine;
    gSinCos(arg0, arg1, arg2);
    line = (struct LineF3 *)gPacketCursor;
    if (gPacketCursor < gPacketLimit) {
        gPacketCursor += 0x1C;
    }
    ctr = 0;
    bias = 0x2C0000;
    __asm__ volatile ("lui $2,%%hi(D_8009AFB4)\n\taddiu %0,$2,%%lo(D_8009AFB4)" : "=r" (points));
    out = line->xy;
    line->tag = 0x4000000;
    line->code = saved_code | 0x20000000;
    c = cosine;
    s = sine;
    for (; ctr < 3; ctr++) {
        out[0] = (origin + (c * points[0] - s * points[1])) >> 16;
        out[1] = (s * points[0] + c * points[1] + bias) >> 16;
        points += 2;
        out += 2;
    }
}
