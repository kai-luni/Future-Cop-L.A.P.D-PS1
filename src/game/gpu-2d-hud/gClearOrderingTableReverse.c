// Categories: Graphics > GPU Command Lists | Platform > GPU
#include "game/types.h"

typedef struct {
    u8 unk00[0x2C];
    void (*unk2C)(s32, s32);
} Unk8009DDE8;

extern u8 gGpuDebugLevel __asm__("D_8009DDF2");
extern  void (*gLogPrintf)(u8 *, s32, s32) __asm__("D_8009DDEC");
extern Unk8009DDE8 *gVramWriteCallback __asm__("D_8009DDE8");
extern u8 gClearOTagFormat[] __asm__("D_80012AD0");
extern u32 gSeqFirstWord[] __asm__("D_8009DEB0");
extern u32 gGpuTpageWord[] __asm__("D_8009DE9C");

void gClearOrderingTableReverse(u32 *arg0, s32 arg1) __asm__("func_80093178");

/**
 * @brief Terminates a primitive list: writes the GPU destination/terminator
 *        word into the list head.
 *
 * @param arg0 Pointer to the first word of the list (for the packet pools this
 *             is the ordering-table base).
 * @param arg1 Number of entries in the list (for the 512-word table this is
 *             arg1 + 2, i.e. the table length).
 *
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * - If the debug level D_8009DDF2 is >= 2 the hook D_8009DDEC is called with
 *   the message D_80012AD0 ("ClearOTagR(%08x,%d)...\n") - a Sony-style
 *   libgpu debug trace.
 * - D_8009DDE8->unk2C(arg0, arg1) performs the actual list clearing (the
 *   function pointer installed by the GPU library).
 * - Finally one word is built: (D_8009DE9C & 0xFFFFFF) | 0x04000000 is stored
 *   to D_8009DEB0 (the GP0 register) and the masked address of D_8009DEB0 is
 *   stored into arg0[0] - the classic "list terminator points at the GPU
 *   register" end tag.
 *
 * @par Globals
 * D_8009DDF2 (debug level), D_8009DDEC (debug hook), D_8009DDE8 (GPU library
 * state, function pointer at +0x2C), D_8009DE9C (command word), D_8009DEB0
 * (GP0 register location).
 *
 * @par Likely role
 * A ClearOTagR-style ordering-table initialiser: arg0 receives the end marker
 * so a later walk terminates.
 *
 * @par Callers
 * func_8001B2C8 and func_8001B4A8, on the two (arg1 + 2)-word tables, once per
 * frame.
 */
void gClearOrderingTableReverse(u32 *arg0, s32 arg1)
{
    register u32 a2 __asm__("$6");
    register u32 a0 __asm__("$4");
    register u32 v1 __asm__("$3");
    register u32 a1 __asm__("$5");
    register u32 *v0 __asm__("$2");

    if (gGpuDebugLevel >= 2) {
        gLogPrintf(gClearOTagFormat, (s32)arg0, arg1);
    }

    gVramWriteCallback->unk2C((s32)arg0, arg1);

    a2 = 0xFFFFFF;
    v0 = arg0;
    __asm__ volatile("" : "+r"(v0));
    a1 = (u32)gSeqFirstWord;
    __asm__ volatile("" : "+r"(a1));
    v1 = (u32)gGpuTpageWord;
    v1 = (v1 & a2);
    a0 = 0x4000000;
    v1 = v1 | a0;
    *(u32 *)a1 = v1;
    a1 = a1 & a2;
    *v0 = a1;
}
