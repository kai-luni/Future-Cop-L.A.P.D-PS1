// Categories: Graphics > 2D Drawing > Lines | Graphics > HUD
#include "game/types.h"

typedef struct {
    u8 unk00[3];
    u8 unk03;
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    s16 unk08;
    s16 unk0A;
    s16 unk0C;
    s16 unk0E;
} Unk770A8;

extern u8 gSegmentFlagTable[] __asm__("D_8009B314");

extern void gDrawSync(s32 arg0) __asm__("func_80092C8C");
extern void gDrawPrimImmediate(u8 *arg0) __asm__("func_80093224");
void gDrawSevenSegmentChar(s32 arg0, s32 arg1, s32 arg2) __asm__("func_800770A8");


/**
 * @brief Draws the selected frame segments around a rectangle (corner and
 *        edge pieces chosen by a flag byte).
 *
 * @param arg0 Left x of the frame.
 * @param arg1 Top y of the frame.
 * @param arg2 Index into the segment flag table D_8009B314.
 * @return Nothing.
 *
 * @par Confirmed
 * The local line descriptor is initialised with type 3, colour 0x40 and
 * RGB 0x80/0x80/0x80. Up to seven segments are emitted, each followed by
 * func_80092C8C(0): bit 0 left edge (y+1..y+5), bit 1 left lower
 * (y+7..y+0xC), bit 2 bottom (x+1..x+4 at y+0xD), bit 8 right lower,
 * bit 0x10 right upper, bit 0x20 top, bit 0x40 middle bar (y+6).
 *
 * @par Likely role
 * The selection-frame drawer of the front end (func_80077278): the flag
 * table encodes which corners/edges each widget state shows, so one
 * routine draws all frame variants.
 *
 * @par Callers
 * func_80077278.
 *
 * @par Environment path
 * Maps an ASCII character (0-9, A-Z, a-z) to a 7-bit segment mask in
 * gSegmentFlagTable and draws each active segment as its own immediate
 * primitive via func_80092C8C and func_80093224 (called by func_80077278:13,23).
 */
void gDrawSevenSegmentChar(s32 arg0, s32 arg1, s32 arg2)
{
    u8 flags;
    Unk770A8 local;

    flags = gSegmentFlagTable[arg2];

    local.unk03 = 3;
    local.unk07 = 0x40;
    local.unk04 = 0x80;
    local.unk05 = 0x80;
    local.unk06 = 0x80;

    gDrawSync(0);

    if (flags & 1) {
        local.unk0A = arg1 + 1;
        local.unk08 = arg0;
        local.unk0C = arg0;
        local.unk0E = arg1 + 5;
        gDrawPrimImmediate((u8 *)&local);
        gDrawSync(0);
    }

    if (flags & 2) {
        local.unk0A = arg1 + 7;
        local.unk08 = arg0;
        local.unk0C = arg0;
        local.unk0E = arg1 + 0xC;
        gDrawPrimImmediate((u8 *)&local);
        gDrawSync(0);
    }

    if (flags & 4) {
        local.unk08 = arg0 + 1;
        local.unk0A = arg1 + 0xD;
        local.unk0C = arg0 + 4;
        local.unk0E = arg1 + 0xD;
        gDrawPrimImmediate((u8 *)&local);
        gDrawSync(0);
    }

    if (flags & 8) {
        local.unk08 = arg0 + 5;
        local.unk0A = arg1 + 7;
        local.unk0C = arg0 + 5;
        local.unk0E = arg1 + 0xC;
        gDrawPrimImmediate((u8 *)&local);
        gDrawSync(0);
    }

    if (flags & 0x10) {
        local.unk08 = arg0 + 5;
        local.unk0A = arg1 + 1;
        local.unk0C = arg0 + 5;
        local.unk0E = arg1 + 5;
        gDrawPrimImmediate((u8 *)&local);
        gDrawSync(0);
    }

    if (flags & 0x20) {
        local.unk08 = arg0 + 1;
        local.unk0A = arg1;
        local.unk0C = arg0 + 4;
        local.unk0E = arg1;
        gDrawPrimImmediate((u8 *)&local);
        gDrawSync(0);
    }

    {
        register u8 *p __asm__("$4");

        p = (u8 *)&local;
        if (flags & 0x40) {
            local.unk08 = arg0 + 1;
            local.unk0A = arg1 + 6;
            local.unk0C = arg0 + 4;
            local.unk0E = arg1 + 6;
            gDrawPrimImmediate(p);
            gDrawSync(0);
        }
    }
}
