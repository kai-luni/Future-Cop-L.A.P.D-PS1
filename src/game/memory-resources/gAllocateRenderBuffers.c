// Categories: Resources > Memory Allocation | Graphics > Render Buffers
#include "game/types.h"

extern s32 gSubScreenActive __asm__("D_8009F5C0");
extern s32 gPacketBufferBases __asm__("D_8009F5C8");
extern s32 gSecondPacketBufferBase __asm__("D_8009F5CC");
extern s32 gRenderNodeTable __asm__("D_8009F5D0");
extern s32 gRenderNodeTableAlt __asm__("D_8009F5D4");
extern s32 gOrderingTableBuffers __asm__("D_8009F5D8");
extern s32 gSecondOrderingTableBuffer __asm__("D_8009F5DC");
extern s32 gPacketPoolBase __asm__("D_8009F5E0");
extern s32 gNodePoolBase __asm__("D_8009F5E4");
extern volatile s32 gUploadRingWriteIdx __asm__("D_8009F5E8");
extern volatile s32 gMdecUploadReclaimIndex __asm__("D_8009F5EC");
extern volatile s32 gMdecUploadSubmitIndex __asm__("D_8009F5F0");
extern s32 gFrameBlockCount __asm__("D_8009F5F4");
extern s32 gNodeCapacity __asm__("D_8009F5F8");
extern s32 gNodeTableLength __asm__("D_8009F5FC");
extern s32 gNodeTableIndex __asm__("D_8009F600");
extern s32 gPacketLimit __asm__("D_8009F9C0");
extern volatile s32 gScreenOffsetValue __asm__("D_8009F9D4");
extern s32 gPrimPoolNext __asm__("D_8009F9F0");
extern s32 gPrimPoolEnd __asm__("D_8009FA28");
extern s32 gPacketCursor __asm__("D_8009FA90");

extern s32 gAllocMemory(s32, s32) __asm__("func_80017EBC");
extern u8 gGetSelectedLayer(void) __asm__("func_8001C034");
extern void gClearOrderingTableReverse(s32, s32) __asm__("func_80093178");

void gAllocateRenderBuffers(s32 arg0, s32 arg1) __asm__("func_8001B2C8");

/**
 * @brief Allocates and initializes the per-scene packet, render-node, and
 * ordering-table buffers.
 *
 * @param arg0 Number of 0x34-byte entries in each packet buffer.
 * @param arg1 Ordering-table count before the two extra words.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Allocates two arrays of arg0 times 0x34 bytes, two arrays of 0x400 times
 * 0x1C bytes, and two arrays of (arg1 + 2) words with tag 2. Installs initial
 * bases and last-entry bounds, initializes the first ordering table, and
 * selects offset 0 or 0x100 by the current layer.
 *
 * @par Globals
 * Writes gSubScreenActive, gRenderNodeTable, gRenderNodeTableAlt,
 * gPacketPoolBase, gNodePoolBase, gUploadRingWriteIdx, gFrameBlockCount,
 * gNodeCapacity, gNodeTableLength, gNodeTableIndex, gPacketLimit,
 * gScreenOffsetValue, gPrimPoolNext, gPrimPoolEnd, and gPacketCursor. Also
 * initializes gPacketBufferBases, gSecondPacketBufferBase,
 * gOrderingTableBuffers, gSecondOrderingTableBuffer, gMdecUploadReclaimIndex,
 * and gMdecUploadSubmitIndex.
 *
 * @par Callers
 * Direct calls in the retail listing are from gLoadFrontEndModule,
 * func_80024A24, gGameMain.
 *
 * @par Role and path
 * Front end, mission, and subsystem initialization prepare these paired
 * buffers for the render builder.
 *
 * @par Unknown
 * The caller supplies the packet capacity and ordering-table extent.
 */
void gAllocateRenderBuffers(s32 arg0, s32 arg1)
{
    s32 allocationSize;
    register s32 value __asm__("$3");
    register s32 offset __asm__("$2");
    register s32 count28 __asm__("$4");

    gFrameBlockCount = arg0;
    gSubScreenActive = 0;
    gNodeCapacity = 0x400;
    gNodeTableLength = arg1;

    gPacketBufferBases = gAllocMemory(gFrameBlockCount * 0x34, 2);

    allocationSize = gFrameBlockCount * 0x34;
    gSecondPacketBufferBase = gAllocMemory(allocationSize, 2);

    allocationSize = gNodeCapacity * 0x1C;
    gRenderNodeTable = gAllocMemory(allocationSize, 2);

    allocationSize = gNodeCapacity * 0x1C;
    gRenderNodeTableAlt = gAllocMemory(allocationSize, 2);

    gOrderingTableBuffers = gAllocMemory((arg1 + 2) * 4, 2);
    gSecondOrderingTableBuffer = gAllocMemory((arg1 + 2) * 4, 2);

    gNodeTableIndex = 0;
    gPrimPoolNext = gPacketBufferBases;
    gPacketPoolBase = gPacketBufferBases;
    gPacketCursor = gRenderNodeTable;
    gNodePoolBase = gRenderNodeTable;

    gClearOrderingTableReverse(gOrderingTableBuffers, gNodeTableLength + 2);

    value = gFrameBlockCount;
    count28 = gNodeCapacity;
    offset = value * 0x34;
    offset -= 0x34;
    value = gPacketPoolBase;
    value += offset;
    gPrimPoolEnd = value;

    offset = count28 * 0x1C;
    offset -= 0x1C;
    value = gNodePoolBase;
    value += offset;
    gPacketLimit = value;

    value = gGetSelectedLayer();
    if (value != 0) {
        value = 0x100;
    }
    offset = 0x100 - value;
    gScreenOffsetValue = offset;
    gMdecUploadReclaimIndex = 0;
    gUploadRingWriteIdx = 0;
    gMdecUploadSubmitIndex = 0;
}
