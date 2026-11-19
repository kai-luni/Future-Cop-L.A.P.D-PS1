// Categories: Game Engine > Object Coordination
#include "game/types.h"
typedef struct Obj { s32 value; u8 pad[24]; s32 id; } Obj;
typedef struct Slot { s8 id; u8 type; u8 active; u8 pad; Obj *a; Obj *b; s32 time; } Slot;
extern Obj *gPendingCvkhNode __asm__("D_8009F270"), *gPendingCvkbNode __asm__("D_8009F274");
extern Slot gObjectSlots[] __asm__("D_800A0A00");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 gFindCshdSoundGroup(s32, s32 *) __asm__("func_80023528");
extern void gFreeBlock(Obj *) __asm__("func_800181D4");
extern s32 gRegisterSoundBank(s32, s32, s32, void *, s32) __asm__("func_80021CB0");
extern void gMarkCvkSlotReady(void) __asm__("func_80023500");

void gRegisterPendingCvkPair(void) __asm__("func_800236FC");

/**
 * @brief Registers matching CVKH and CVKB nodes as a pending sound bank.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Requires both pending pointers and equal IDs at +0x1C, then checks the CSHD group and
 * searches eight slots by their -1 bank marker. Frees both nodes on validation or capacity
 * failure. A successful registration stores both nodes, state 1 and current tick plus 1500
 * before clearing the pending pointers.
 *
 * @par Globals
 * Reads and clears gPendingCvkhNode and gPendingCvkbNode, reads gFrameTickCounter and may
 * populate gObjectSlots.
 *
 * @par Callers
 * Direct calls from gRegisterCvkChunk (func_80023930).
 *
 * @par Role and path
 * Runs when the CVK loader delivers tagged bank chunks.
 *
 * @par Unknown
 * The negative result from the bank helper clears the pending pointers through a different path
 * from validation failure.
 */
void gRegisterPendingCvkPair(void)
{
    u16 i = 0;
    u8 failed = 0;
    s32 result;
    s32 time;
    if (gPendingCvkhNode && gPendingCvkbNode) {
        if (gPendingCvkhNode->id == gPendingCvkbNode->id && gFindCshdSoundGroup(gPendingCvkhNode->id, 0)) {
            while (i < 8) {
                if (gObjectSlots[i].id == -1) break;
                i++;
            }
            if (i >= 8) failed = 1;
        } else failed = 1;
        if (failed) {
            gFreeBlock(gPendingCvkhNode);
            gFreeBlock(gPendingCvkbNode);
        } else {
            result = gRegisterSoundBank(gPendingCvkhNode->value, gPendingCvkbNode->value, -1, gMarkCvkSlotReady, i);
            if (result >= 0) {
                gObjectSlots[i].active = 1;
                time = gFrameTickCounter + 1500;
                gObjectSlots[i].id = result;
                gObjectSlots[i].time = time;
                gObjectSlots[i].type = gPendingCvkhNode->id;
                gObjectSlots[i].a = gPendingCvkhNode;
                gObjectSlots[i].b = gPendingCvkbNode;
            }
        }
        gPendingCvkbNode = 0;
        gPendingCvkhNode = 0;
    }
}
