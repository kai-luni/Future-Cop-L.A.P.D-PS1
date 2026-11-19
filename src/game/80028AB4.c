// Categories: Game Engine > Queued Operations
#include "game/types.h"

typedef struct Node {
    s32 unk00;
    s32 unk04;
    u8 pad08[8];
    u8 data[1];
} Node;
typedef struct State {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    u8 pad0c[0x10];
    s32 unk1c;
    u8 unk20;
} State;
extern State gStreamBufferList __asm__("D_800A0B70");
extern Node *gModuleReadPendingBuffers[] __asm__("D_800A0C20");
extern volatile s32 gModuleReadPendingIndex __asm__("D_8009F6B4");
extern Node * volatile gModuleReadActiveBuffer __asm__("D_8009F6AC");
extern u8 gModuleReadBusy __asm__("D_8009F6A8");
extern void gSeekFileSlot(s32, s32, s32) __asm__("func_8001D6D0");
extern void gReadFileSlot(s32, void *, s32, void (*)()) __asm__("func_8001DB60");
extern void gCompleteModuleBufferRead() __asm__("func_80028918");

void gStartNextModuleBufferRead(void) __asm__("func_80028AB4");

/**
 * @brief Dequeues the next module buffer and starts its file-slot read.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Requires an occupied queue cell, a clear stop byte and a nonnegative file slot. Clears the
 * cell and wraps its read index at four, saves the file position on the active node, sets busy
 * and calls the seek and read routines with the completion callback.
 *
 * @par Globals
 * Reads gStreamBufferList and gModuleReadPendingBuffers. Updates gModuleReadPendingIndex,
 * gModuleReadActiveBuffer and gModuleReadBusy.
 *
 * @par Callers
 * Direct calls from gCompleteModuleBufferRead (D_80028918), gServiceModuleReadQueues
 * (func_80028E58).
 *
 * @par Role and path
 * The queue service and read callback use this to keep the module stream supplied.
 *
 * @par Unknown
 * The file-slot routines own the actual I/O scheduling.
 */
void gStartNextModuleBufferRead(void)
{
    Node *node;
    State *state;
    state = &gStreamBufferList;
    node = gModuleReadPendingBuffers[gModuleReadPendingIndex];
    if (node != 0 && state->unk20 == 0 && gStreamBufferList.unk00 >= 0) {
        gModuleReadPendingBuffers[gModuleReadPendingIndex] = 0;
        gModuleReadPendingIndex++;
        if (gModuleReadPendingIndex == 4) gModuleReadPendingIndex = 0;
        gModuleReadActiveBuffer = node;
        gModuleReadActiveBuffer->unk04 = state->unk08;
        gModuleReadBusy = 1;
        gSeekFileSlot(gStreamBufferList.unk00, state->unk08, 0);
        gReadFileSlot(gStreamBufferList.unk00, node->data, state->unk1c, gCompleteModuleBufferRead);
    }
}
