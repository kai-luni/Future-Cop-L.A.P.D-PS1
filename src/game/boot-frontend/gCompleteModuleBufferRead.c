// Categories: Game Engine > Node Queues
#include "game/types.h"

typedef struct Node {
    struct Node *next;
    s32 unk04;
    s32 unk08;
    s32 count;
    s32 unk10;
} Node;
typedef struct State {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0c;
    Node *head;
    u8 pad14[8];
    s32 unk1c;
    u8 unk20;
    u8 pad21[0x1b];
    u8 unk3c;
    u8 pad3d[0xf];
    s32 unk4c;
} State;
extern State gStreamBufferList __asm__("D_800A0B70");
extern Node *gModuleReadCompletedBuffers[] __asm__("D_800A0C10");
extern u8 gModuleReadBusy __asm__("D_8009F6A8");
extern Node * volatile gModuleReadActiveBuffer __asm__("D_8009F6AC");
extern volatile s32 gModuleReadCompleteWriteIndex __asm__("D_8009F6B0");
extern s32 gModuleReadEndCheck __asm__("D_8009F6E8");
extern void gRecycleModuleReadBuffer(Node *) __asm__("func_800288BC");
extern void gStartNextModuleBufferRead(void) __asm__("func_80028AB4");

void gCompleteModuleBufferRead(s32 arg0, s32 arg1) __asm__("func_80028918");

/**
 * @brief Validates a module file read and queues or recycles the buffer.
 *
 * @param arg0 Reported amount used to validate and advance the position.
 * @param arg1 Reported status, subject to overrides by the validation checks.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears busy immediately for a negative file slot. Otherwise checks reported size and saved
 * offset, forces status 0xDEAD on an offset mismatch and recognizes the terminal position.
 * Successful reads advance the file position and queue the node or recycle it for the tagged
 * stop case. Clears the current node and busy flag and starts another read when allowed.
 *
 * @par Globals
 * Uses gStreamBufferList, gModuleReadActiveBuffer, gModuleReadBusy, gModuleReadEndCheck,
 * gModuleReadCompletedBuffers and gModuleReadCompleteWriteIndex.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * Installed by gStartNextModuleBufferRead as the file-slot completion callback at retail alias
 * gCompleteModuleBufferRead.
 *
 * @par Unknown
 * The full meaning of tag 0x53575652 in every stream type is unresolved.
 */
void gCompleteModuleBufferRead(s32 arg0, s32 arg1)
{
    register State *state asm("$16");
    u8 flag;
    u8 final_flag;
    s32 value;
    Node *node;
    register s32 pos asm("$6");
    register s32 node_value asm("$2");
    if (gStreamBufferList.unk00 < 0) {
        gModuleReadBusy = 0;
        return;
    }
    state = &gStreamBufferList;
    if (arg0 != state->unk1c) arg1 = 5;
    pos = state->unk08;
    node_value = gModuleReadActiveBuffer->unk04;
    if (pos != node_value) {
        arg1 = 0xdead;
    } else if ((node_value = arg0 + pos) == state->unk04) {
        arg1 = 0;
        state->unk20 = 1;
        gModuleReadEndCheck = 0;
    }
    if (arg1 == 0) {
        state->unk08 += arg0;
        if (gModuleReadEndCheck != 0 && state->unk08 == state->unk4c) {
            gModuleReadEndCheck = 0;
            state->unk20 = 1;
        }
        flag = 0;
        if (gModuleReadActiveBuffer->unk10 == 0x53575652 && state->unk0c != -1) {
            if (state->unk3c != 0) {
                flag = 1;
                node = gModuleReadActiveBuffer;
                __asm__("addu %0,%1,$zero" : "=r"(value) : "r"(flag), "r"(node));
                state->unk20 = value;
                gRecycleModuleReadBuffer(node);
            } else {
                state->unk3c = 1;
            }
        }
        if (!flag) {
            gModuleReadCompletedBuffers[gModuleReadCompleteWriteIndex] = gModuleReadActiveBuffer;
            gModuleReadCompleteWriteIndex++;
            if (gModuleReadCompleteWriteIndex == 4) gModuleReadCompleteWriteIndex = 0;
        }
    } else {
        gRecycleModuleReadBuffer(gModuleReadActiveBuffer);
    }
    final_flag = *(volatile u8 *)&state->unk20;
    gModuleReadActiveBuffer = 0;
    gModuleReadBusy = 0;
    if (!final_flag) gStartNextModuleBufferRead();
}
