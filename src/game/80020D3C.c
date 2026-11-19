// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

extern u32 *gOperationState __asm__("D_8009AB08");
extern u8 gCardDevicePathFormat[] __asm__("D_80010B0C");

extern s32 gFindCardFileSlot(void *arg0) __asm__("func_80020C90");
extern void func_8008CD30(void *buf, void *fmt, u32 n, void *arg);
extern void gQueueMcOpMode2(s32 a, s32 b, s32 c, s32 d) __asm__("func_8001F1B8");
extern s32 gGetMcPendingFlag(void) __asm__("func_8001F37C");

s32 gQueueCardFileRead(void *arg0, s32 arg1, u32 arg2) __asm__("func_80020D3C");

/**
 * @brief Queues a memory-card read for a named file and reports the accepted
 * sector count.
 *
 * @param arg0 Filename to look up.
 * @param arg1 Destination RAM address.
 * @param arg2 Requested byte count.
 * @return Accepted sector count in 128-byte units, or zero.
 *
 * @par Confirmed mechanics
 * Returns zero for zero size or slot zero. Formats a device-qualified filename
 * into a stack buffer, then calls gQueueMcOpMode2 with slot times 64,
 * ceil(bytes/128), the destination, and zero. Returns the rounded count if the
 * pending flag remains clear.
 *
 * @par Globals
 * Reads gOperationState and gCardDevicePathFormat.
 *
 * @par Callers
 * Direct calls in the retail listing are from gShowSaveMenuReading.
 *
 * @par Role and path
 * The save-loading path looks up the file before staging the card read.
 *
 * @par Unknown
 * The stack-formatted path is not passed to the subsequent queue call.
 */
s32 gQueueCardFileRead(void *arg0, s32 arg1, u32 arg2)
{
    u8 buf[0x100];
    s32 ret;
    s32 r;
    s32 n;

    ret = 0;
    if (arg2 != 0) {
        r = gFindCardFileSlot(arg0);
        if (r != 0) {
            func_8008CD30(buf, gCardDevicePathFormat, *gOperationState >> 4, arg0);
            n = (arg2 + 0x7F) >> 7;
            gQueueMcOpMode2(r << 6, n, arg1, ret);
            if (gGetMcPendingFlag() == 0) {
                ret = n;
            }
        }
    }
    return ret;
}
