// Categories: Graphics > Effects > Object Pool | Game Engine > Entity Lifecycle
#include "game/types.h"

struct Unk5E074 {
    void *model;
};

extern void func_8006CA68(void *model);
extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Releases an attached effect: unbinds its model and frees the
 *        effect block.
 *
 * @param effect Effect record; +0 the model pointer.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8006CA68(effect->model) releases the model reference,
 * func_800181D4(effect) frees the block to the upper allocator.
 *
 * @par Likely role
 * The expire path of the effect driver func_8005E0AC (when the per-frame
 * update returns anything but 1) and of func_8005E22C.
 *
 * @par Callers
 * func_8005E0AC, func_8005E22C.
 */
void func_8005E074(struct Unk5E074 *effect)
{
    func_8006CA68(effect->model);
    gFreeBlock(effect);
}
