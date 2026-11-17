// Categories: Graphics > GPU Primitives > Submit | Graphics > 2D Drawing
#include "game/types.h"

typedef struct {
    u8 unk00[0x14];
    void (*unk14)(u8 *, s32);
    u8 unk18[0x24];
    void (*unk3C)(s32);
} Unk8009DDE8;

extern Unk8009DDE8 *gVramWriteCallback __asm__("D_8009DDE8");

/**
 * @brief Submits a line/rect packet through the active draw callback table.
 *
 * @param arg0 Packet: byte 3 the length word, +4 the payload.
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009DDE8->unk3C(0) runs first (the table's flush/select entry), then
 * D_8009DDE8->unk14(arg0 + 4, arg0[3]) submits the payload with its length.
 *
 * @par Likely role
 * The 2D submit primitive behind the frame-segment drawer func_800770A8:
 * the callback table D_8009DDE8 is the same one the debug capture variants
 * install, so lines route to whichever sink is active.
 *
 * @par Callers
 * func_800770A8.
 */
void func_80093224(u8 *arg0)
{
    s32 temp = arg0[3];

    gVramWriteCallback->unk3C(0);
    gVramWriteCallback->unk14(arg0 + 4, temp);
}
