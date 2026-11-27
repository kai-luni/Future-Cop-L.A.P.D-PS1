// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

extern void gInitFromSourceModel(void *, void *, void *) __asm__("func_800317BC");
extern s32 gEntityBaseHandler(s32, void *, void *, void *) __asm__("func_8002AD58");
s32 gHandleSourceModelEntityMsg(u32 message, void *object, void *data, void *arg) __asm__("func_8003190C");


/**
 * @brief Message table of the mountable-model entity class: size query, model
 *        attachment, two no-op cases and the base fallback.
 *
 * The same contract as func_8003A844 and the base handler func_8002AD58. Message
 * 1 answers the block size, message 2 forwards to func_800317BC - which first
 * asks the base for its behaviour, then attaches the donor model and copies the
 * frame vertex - and messages 4 and 6 answer 0. Case 2 has no break and falls
 * through into 4 and 6, so a successful attachment also returns 0.
 *
 * @param message Handler case number.
 * @param object Entity block.
 * @param data Case payload; for message 2 the descriptor carrying the donor
 *        model and matrix.
 * @param arg Fourth word, forwarded unchanged.
 * @return 0xA0 for case 1, 0 for cases 2/4/6, otherwise the base handler's result
 *         (-1 for its default case).
 *
 * @par Globals
 * None touched here directly; the callees work on the entity.
 *
 * @par Control flow
 * Switch with four labels and a default; the deliberate fall-through from case 2
 * into case 4/6 is why the attachment returns 0.
 *
 * @par Calls
 * func_800317BC - model attachment for message 2.
 * func_8002AD58 - base-class handler for every other message.
 *
 * @par Side effects
 * Only through the callees (model at +0x28, matrix, frame vertex).
 *
 * @par Callers
 * Unknown: no direct jal. Confirmed reference: func_80031690 stores the address in
 * the handler word of a node request (+0x10 of a stack block) at 0x80031704, so it
 * is reached through that request.
 */
s32 gHandleSourceModelEntityMsg(u32 message, void *object, void *data, void *arg)
{
    switch (message) {
    case 1:
        return 0xA0;
    case 2:
        gInitFromSourceModel(object, data, arg);
    case 4:
    case 6:
        return 0;
    default:
        return gEntityBaseHandler(message, object, data, arg);
    }
}
