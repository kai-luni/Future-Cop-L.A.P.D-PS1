// Categories: Resources > Save Data | Platform > Memory Card
#include "game/types.h"

extern u8 *gOperationState __asm__("D_8009AB08");

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern int func_80020610(void);

/**
 * @brief Initialises the memory-card manager inside a caller-supplied block.
 *
 * @param arg0 Channel selector; only 0 and 1 are accepted and the value is
 *             stored shifted left by 4 in the first word of the block.
 * @param arg1 Block of func_80020610() (0x1BB8) bytes.
 * @return arg1 on success, 0 when a manager is already installed or arg0 >= 2.
 *
 * @par Confirmed flow
 * The block is zeroed, then the 16 slot pointers at +0x44 and the 16 directory
 * pointers at +0x84 are filled with the 0x80-byte frames at +0xC4 and +0x104.
 *
 * @par Globals
 * D_8009AB08 - receives arg1 (memory-card manager context).
 *
 * @par Callers
 * func_8007F9CC (profile open).
 */
void *func_80020618(s32 arg0, void *arg1)
{
    u32 i;
    u8 *result;

    if (gOperationState == 0) {
        if ((u32)arg0 < 2) {
            gOperationState = arg1;
            func_8001E4C0(gOperationState, 0, func_80020610());

            for (i = 0; i < 0x10; i++) {
                ((u32 *)(gOperationState + 0x44))[i] =
                    (u32)(gOperationState + 0xC4 + i * 0x80);
                ((u32 *)(gOperationState + 0x84))[i] =
                    (u32)(gOperationState + 0x104 + i * 0x80);
            }

            result = gOperationState;
            *(u32 *)result = (u32)arg0 << 4;
            return result;
        }
    }

    return 0;
}
