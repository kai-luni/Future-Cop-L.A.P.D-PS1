// Categories: Game Engine > Node Queues
#include "game/types.h"
typedef struct Node { struct Node *next; } Node;
typedef struct State { u8 pad[0x10]; Node *pending; Node *head; u8 pad18[8]; u8 flag; } State;
extern State gStreamBufferList __asm__("D_800A0B70");
extern Node *gModuleReadPendingBuffers[] __asm__("D_800A0C20");
extern Node *gModuleReadCompletedBuffers[] __asm__("D_800A0C10");
extern s32 gModuleReadPendingWriteIndex __asm__("D_8009F6A4");
extern s32 gModuleReadCompleteIndex __asm__("D_8009F6A0");
extern s32 gModuleReadPendingIndex __asm__("D_8009F6B4");
extern u8 gModuleReadBusy __asm__("D_8009F6A8");
extern void gStartNextModuleBufferRead(void) __asm__("func_80028AB4");

Node *gServiceModuleReadQueues(u8 arg) __asm__("func_80028E58");

/**
 * @brief Submits reusable module buffers for reading and optionally collects completed reads.
 *
 * @param arg Nonzero to append completed nodes to the head list.
 * @return The current head of the state list.
 *
 * @par Confirmed mechanics
 * When the stop byte is clear, drains the reusable list into the four-cell pending ring and
 * starts a read if the selected cell is occupied and the loader is idle. A nonzero low byte of
 * the argument drains the completion ring into the parse list, wrapping each ring index at
 * four.
 *
 * @par Globals
 * Updates gStreamBufferList, gModuleReadPendingBuffers, gModuleReadPendingWriteIndex,
 * gModuleReadCompletedBuffers and gModuleReadCompleteIndex. Reads gModuleReadPendingIndex and
 * gModuleReadBusy.
 *
 * @par Callers
 * Direct calls from gParseStreamedModuleChunks (func_8002902C).
 *
 * @par Role and path
 * The module parser services the asynchronous file queues before or between parse passes.
 *
 * @par Unknown
 * The ring capacity relies on the loader's buffer allocation.
 */
Node *gServiceModuleReadQueues(u8 arg)
{
    State *state = &gStreamBufferList;
    Node *node;
    Node *p;
    if (state->flag == 0) {
        while ((node = state->pending) != 0) {
            state->pending = node->next;
            node->next = 0;
            gModuleReadPendingBuffers[gModuleReadPendingWriteIndex] = node;
            gModuleReadPendingWriteIndex++;
            if (gModuleReadPendingWriteIndex == 4) gModuleReadPendingWriteIndex = 0;
        }
        if (gModuleReadPendingBuffers[gModuleReadPendingIndex] != 0 && gModuleReadBusy == 0) gStartNextModuleBufferRead();
    }
    if (arg) {
        while ((node = gModuleReadCompletedBuffers[gModuleReadCompleteIndex]) != 0) {
            gModuleReadCompletedBuffers[gModuleReadCompleteIndex] = 0;
            gModuleReadCompleteIndex++;
            if (gModuleReadCompleteIndex == 4) gModuleReadCompleteIndex = 0;
            node->next = 0;
            p = state->head;
            if (p == 0) state->head = node;
            else {
                while (p->next != 0) p = p->next;
                p->next = node;
            }
        }
    }
    return state->head;
}
