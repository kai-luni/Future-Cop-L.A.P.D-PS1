// Categories: Resources > Memory Allocation | Graphics > Render Buffers
#include "game/types.h"

extern s32 gSubScreenActive __asm__("D_8009F5C0");
extern s32 D_8009F5C8;
extern s32 D_8009F5CC;
extern s32 gRenderNodeTable __asm__("D_8009F5D0");
extern s32 gRenderNodeTableAlt __asm__("D_8009F5D4");
extern s32 D_8009F5D8;
extern s32 D_8009F5DC;
extern s32 gPacketPoolBase __asm__("D_8009F5E0");
extern s32 gNodePoolBase __asm__("D_8009F5E4");
extern volatile s32 D_8009F5E8;
extern volatile s32 D_8009F5EC;
extern volatile s32 D_8009F5F0;
extern s32 gFrameBlockCount __asm__("D_8009F5F4");
extern s32 D_8009F5F8;
extern s32 gNodeTableLength __asm__("D_8009F5FC");
extern s32 gNodeTableIndex __asm__("D_8009F600");
extern s32 gPacketLimit __asm__("D_8009F9C0");
extern volatile s32 gScreenOffsetValue __asm__("D_8009F9D4");
extern s32 gPrimPoolNext __asm__("D_8009F9F0");
extern s32 gPrimPoolEnd __asm__("D_8009FA28");
extern s32 gPacketCursor __asm__("D_8009FA90");

extern s32 func_80017EBC(s32, s32);
extern u8 func_8001C034(void);
extern void func_80093178(s32, s32);

/**
 * @brief Allocates the per-level render arena: two packet pools, two
 *        fixed-stride record arrays and two small word tables, and resets their
 *        cursors.
 *
 * @param arg0 Number of entries in the packet pool (each entry is 0x34 = 52
 *             bytes). Callers pass 0xC8 (200).
 * @param arg1 Entry count minus two for the two word tables: the allocation is
 *             (arg1 + 2) * 4 bytes each. Callers pass 0x200 (512).
 *
 * @return Nothing; every pointer is stored in a global.
 *
 * @par Confirmed mechanics
 * - D_8009F5C8/D_8009F5CC: two blocks of arg0 * 0x34 bytes (mode 2 =
 *   allocate from the front of the free-range table).
 * - D_8009F5D0/D_8009F5D4: two blocks of 0x400 * 0x1C bytes (1024 entries of
 *   28 bytes).
 * - D_8009F5D8/D_8009F5DC: two blocks of (arg1 + 2) * 4 bytes.
 * - D_8009F9F0/D_8009F5E0 are set to the start of the first packet block,
 *   D_8009FA90/D_8009F5E4 to the start of the first 28-byte block;
 *   D_8009FA28 and D_8009F9C0 are set to the *last* entry of those blocks
 *   (base + count * stride - stride), giving the bump allocators their
 *   inclusive limit.
 * - func_80093178(tables, arg1 + 2) writes the block address into the first
 *   table word (see that function).
 * - func_8001C034() returns a screen offset flag; D_8009F9D4 is set to
 *   0x100 - (flag ? 0x100 : 0), i.e. 0 or 0x100, and the three counters
 *   D_8009F5E8/D_8009F5EC/D_8009F5F0 are cleared.
 *
 * @par Likely role (indirect evidence)
 * D_8009F9F0..D_8009FA28 is used throughout the renderer as a 52-byte GPU
 * packet bump allocator (see func_800164F4, which fills tag/colour/vertex/UV
 * fields of exactly that layout), and D_8009F5D0 is a 1024-entry array of
 * 28-byte records. The normal/free pair (C8/CC, D0/D4, D8/DC) matches the
 * double-buffered frame scheme used by the rest of the engine.
 *
 * @par Globals
 * D_8009F5C0 (counter, cleared), D_8009F5F4 (entry count), D_8009F5F8 (0x400,
 * 28-byte record count), D_8009F5FC (table length), D_8009F600 (cleared),
 * D_8009F9F0/D_8009FA28/D_8009FA90/D_8009F9C0 (pool cursors/limits),
 * D_8009F5E0/D_8009F5E4 (pool bases), D_8009F9D4 (screen offset).
 *
 * @par Calls
 * func_80017EBC(size, 2) is the shared allocator (mode 2 = front of the free
 * range); func_8001C034() supplies the screen offset flag; func_80093178()
 * patches the table head.
 *
 * @par Callers
 * func_80024904 and func_80024A24 (the two front-end module loaders) and
 * func_80076A2C, each time a screen module is started.
 */
void func_8001B2C8(s32 arg0, s32 arg1)
{
    s32 allocationSize;
    register s32 value __asm__("$3");
    register s32 offset __asm__("$2");
    register s32 count28 __asm__("$4");

    gFrameBlockCount = arg0;
    gSubScreenActive = 0;
    D_8009F5F8 = 0x400;
    gNodeTableLength = arg1;

    D_8009F5C8 = func_80017EBC(gFrameBlockCount * 0x34, 2);

    allocationSize = gFrameBlockCount * 0x34;
    D_8009F5CC = func_80017EBC(allocationSize, 2);

    allocationSize = D_8009F5F8 * 0x1C;
    gRenderNodeTable = func_80017EBC(allocationSize, 2);

    allocationSize = D_8009F5F8 * 0x1C;
    gRenderNodeTableAlt = func_80017EBC(allocationSize, 2);

    D_8009F5D8 = func_80017EBC((arg1 + 2) * 4, 2);
    D_8009F5DC = func_80017EBC((arg1 + 2) * 4, 2);

    gNodeTableIndex = 0;
    gPrimPoolNext = D_8009F5C8;
    gPacketPoolBase = D_8009F5C8;
    gPacketCursor = gRenderNodeTable;
    gNodePoolBase = gRenderNodeTable;

    func_80093178(D_8009F5D8, gNodeTableLength + 2);

    value = gFrameBlockCount;
    count28 = D_8009F5F8;
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

    value = func_8001C034();
    if (value != 0) {
        value = 0x100;
    }
    offset = 0x100 - value;
    gScreenOffsetValue = offset;
    D_8009F5EC = 0;
    D_8009F5E8 = 0;
    D_8009F5F0 = 0;
}
