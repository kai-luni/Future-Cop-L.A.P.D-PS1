// Categories: Graphics > GPU > VRAM Transfer | Platform > GPU
#include "game/types.h"

typedef struct {
    u8 unk00[0x8];
    void (*unk08)(s32, s32, s32, s32);
    u8 unk0C[0x10];
    s32 unk1C;
} Unk8009DDE8;

extern u8 gDebugTagArg[] __asm__("D_80012AA0");
extern Unk8009DDE8 *gVramWriteCallback __asm__("D_8009DDE8");

extern void func_80092CF4(u8 *arg0, s32 arg1);

/**
 * @brief Sends an 8-word command block to the GPU library with a debug
 *        trace.
 *
 * @param arg0 Command/data word forwarded to the library.
 * @param arg1 Second library parameter.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80092CF4(D_80012AA0, arg0) (debug hook with a tag string), then
 * D_8009DDE8->unk08(D_8009DDE8->unk1C, arg0, 8, arg1) - the library
 * callback with context, payload, length 8 and the extra parameter.
 * Same dispatch shape as func_80092E10.
 *
 * @par Callers
 * No direct callers in the call graph; referenced through a pointer
 * table.
 */
void func_80092F98(s32 arg0, s32 arg1)
{
    func_80092CF4(gDebugTagArg, arg0);
    gVramWriteCallback->unk08(gVramWriteCallback->unk1C, arg0, 8, arg1);
}
