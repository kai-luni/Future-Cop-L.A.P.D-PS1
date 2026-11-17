// Categories: Resources > Movie Playback | Platform > MDEC
#include "game/types.h"

extern u8 *gOperationState __asm__("D_8009AB08");

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern int func_80020610(void);

/**
 * @brief Creates the movie decoder context inside a caller-provided block.
 *
 * @param arg0 Channel/mode selector; only values 0 and 1 are accepted.
 * @param arg1 Block of at least 0x1BB8 bytes (see func_80020610).
 *
 * @return The context pointer on success, NULL when a context already exists
 *         (D_8009AB08 != 0) or the selector was out of range.
 *
 * @par Confirmed flow
 * 1. If no context exists and arg0 < 2: store arg1 in D_8009AB08 and clear
 *    0x1BB8 bytes.
 * 2. Two tables of 16 pointers are built: at +0x44 they point to
 *    base + 0xC4 + i * 0x80, at +0x84 to base + 0x104 + i * 0x80. Those are
 *    two 16-entry, 0x80-byte-stride buffer rings (2 KB each).
 * 3. The first word receives arg0 << 4 (channel number in the upper nibble).
 *
 * @par Globals
 * D_8009AB08 (the single decoder context).
 *
 * @par Likely role
 * The MDEC/XA context set-up for the movie player: the two pointer rings are
 * the input/output frame queues the decode step rotates through.
 *
 * @par Callers
 * func_8007F9CC only.
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
