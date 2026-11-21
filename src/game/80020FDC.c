// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

struct Unk8009AB08 {
    u32 unk0000;
    u8 unk0004[0x8F4];
    s32 unk8F8;
    u8 unk8FC[0x10];
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    u8 unk9A0[4];
    s32 unk9A4;
    u8 unk9A8[0x120C];
    s32 unk1BB4;
};

extern struct Unk8009AB08 *gOperationState __asm__("D_8009AB08");
extern char gCardDevicePathFormat[] __asm__("D_80010B0C");

extern s32 gSpillVarargsArguments(char *dst, char *fmt, u32 arg2, u32 arg3) __asm__("func_8008CD30");
extern s32 gFindCardFileSlot(void *arg0) __asm__("func_80020C90");
extern void gQueueMcOpMode3(void *arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_8001F24C");
extern s32 gFindFreeCardBlockRun(s32 arg0) __asm__("func_80020E68");
extern void gBuildCardFileDirectoryFrame(char *text, s32 index) __asm__("func_80020F34");
extern void gQueueMcOpMode6(u32 arg0, u32 arg1, u32 arg2, u32 arg3) __asm__("func_8001F2E0");
extern u32 gGetMcPendingFlag(void) __asm__("func_8001F37C");

s32 gQueueCardFileWrite(u32 arg0, u32 arg1, u32 arg2) __asm__("func_80020FDC");

/**
 * @brief Queues a memory-card write, using a free directory run when the named
 * file does not exist.
 *
 * @param arg0 Filename string supplied as an address word.
 * @param arg1 Address of the data to write.
 * @param arg2 Transfer length in bytes.
 * @return 0x2000 when the pending check is clear after queueing, or zero if no
 *     slot is obtained or the pending check fails.
 *
 * @par Confirmed mechanics
 * Formats a device path and rounds the transfer length up to 128-byte sectors.
 * An existing slot takes queue mode 3. Otherwise finds free slots, initializes
 * the directory frame, and takes queue mode 6. Both pass slot times 64 to the
 * request helper.
 *
 * @par Globals
 * Reads gOperationState and gCardDevicePathFormat.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80080B70.
 *
 * @par Role and path
 * The save UI uses this function to stage a write for the named file.
 *
 * @par Unknown
 * The manager completes the write later. The return value alone contains no
 * transferred byte count.
 */
s32 gQueueCardFileWrite(u32 arg0, u32 arg1, u32 arg2)
{
    register u32 p0 __asm__("$18") = arg0;
    register u32 p1 __asm__("$21") = arg1;
    register u32 p2 __asm__("$20") = arg2;
    register s32 result __asm__("$17") = 0;
    register u32 idx __asm__("$19");
    register u32 slot __asm__("$16");
    register u32 tmp __asm__("$2");
    register u32 a0 __asm__("$4");
    char buf[0x100];

    __asm__("" : "=r"(p0) : "0"(p0));
    __asm__("" : "=r"(p1) : "0"(p1));
    __asm__("" : "=r"(p2) : "0"(p2));
    __asm__("" : "=r"(result) : "0"(result));

    gSpillVarargsArguments(buf, gCardDevicePathFormat, gOperationState->unk0000 >> 4, p0);
    a0 = p0;
    __asm__("" : "=r"(a0) : "0"(a0));
    tmp = p2 + 0x7F;
    idx = tmp >> 7;
    slot = gFindCardFileSlot(a0);
    if (slot != 0) {
        gQueueMcOpMode3(slot << 6, idx, p1, result);
        goto tail;
    }
    slot = gFindFreeCardBlockRun(p2);
    if (slot == 0) {
        return result;
    }
    gBuildCardFileDirectoryFrame(p0, slot);
    gQueueMcOpMode6(slot << 6, idx, p1, 0);
tail:
    result = (gGetMcPendingFlag() < 1) << 13;
    __asm__("" : "=r"(result) : "0"(result));
    return result;
}
