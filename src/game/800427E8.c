// Categories: Resources > Memory Management | Gameplay > Effects
#include "game/types.h"

struct Unk427E8 {
    u8 unk00[0x6C];
    void *unk6C;
    u8 unk70[0x34];
    s16 unkA4;
    u8 unkA6[0x2];
    void *unkA8;
};

extern s32 gRemainingCounter __asm__("D_8009FAB0");
extern void func_800181D4(void *block);

/**
 * @brief Releases an effect object and its two owned blocks, adjusting
 *        the live counter.
 *
 * @param object Object with the size/count field unkA4, an optional block
 *               unkA8 and the main block unk6C.
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009FAB0 -= object->unkA4; when unkA8 != 0 it is freed via
 * func_800181D4, then unk6C and the object itself are freed the same way.
 *
 * @par Callers
 * func_80044610 (effect dispatcher).
 */
void func_800427E8(struct Unk427E8 *object)
{
    gRemainingCounter -= object->unkA4;
    if (object->unkA8 != 0) {
        func_800181D4(object->unkA8);
    }
    func_800181D4(object->unk6C);
    func_800181D4(object);
}
