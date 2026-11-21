// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

struct CardManager {
    s32 unk00;
    s32 state[16];
    void *primary[16];
};

extern struct CardManager *gOperationState __asm__("D_8009AB08");

extern s32 gIsCardResultUsable(s32 a0) __asm__("func_80020A9C");
extern void gParseCardDirectory(void) __asm__("func_800207CC");
extern s32 gBiosStrcmp(void *arg0, void *arg1) __asm__("func_8008CCA0");

s32 gFindCardFileSlot(void *arg0) __asm__("func_80020C90");

/**
 * @brief Finds a memory-card directory slot whose stored filename matches the
 * supplied name.
 *
 * @param arg0 Pointer to the filename string.
 * @return The matching slot index, or zero on a failed check or miss.
 *
 * @par Confirmed mechanics
 * Checks gIsCardResultUsable, refreshes slots with gParseCardDirectory, and
 * compares the primary name of each state-1 slot with the supplied name. Stops
 * on the first comparison that returns zero.
 *
 * @par Globals
 * Reads gOperationState and its slot state and primary name arrays.
 *
 * @par Callers
 * Direct calls in the retail listing are from gQueueCardFileRead,
 * gQueueCardFileWrite, func_8007FB04.
 *
 * @par Role and path
 * The card read, write, and UI paths use the resulting index to find a save
 * file.
 *
 * @par Unknown
 * Zero also represents slot 0, so callers cannot distinguish it from a miss
 * here.
 */
s32 gFindCardFileSlot(void *arg0)
{
    register s32 ret asm("s1") = 0;
    register u32 i asm("s0");
    register s32 one asm("s2");

    if (gIsCardResultUsable(1) != 0) {
        gParseCardDirectory();
        __asm__("" : "=r"(ret) : "0"(ret));
        i = ret;
        one = 1;
        do {
            if (gOperationState->state[i] == one) {
                if (gBiosStrcmp(arg0, gOperationState->primary[i]) == 0) {
                    ret = i;
                    break;
                }
            }
            i++;
        } while (i < 16);
    }
    return ret;
}
