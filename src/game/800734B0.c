// Categories: Game Engine > Init | Graphics > Model Renderer > Scene Traversal
#include "game/types.h"

struct Unk734B0;

extern struct Unk734B0 *gBeamNodeList __asm__("D_8009F38C");
void gClearBeamNodeList(void) __asm__("func_800734B0");


/**
 * @brief Clears the global model/scene-graph node list head D_8009F38C during mission boot.
 *
 * Stores 0 into D_8009F38C at the end of func_80024A24, resetting the singly linked render list later walked by func_80073700 and populated by func_8007377C. Category Game Engine > Init | Graphics > Model Renderer > Scene Traversal reflects the boot initializer and scene list head.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F38C - head pointer of the active model/scene-graph linked list, cleared to 0.
 *
 * @par Control flow
 * Straight-line store of 0 and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F38C = 0.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024CF4 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void gClearBeamNodeList(void)
{
    gBeamNodeList = 0;
}
