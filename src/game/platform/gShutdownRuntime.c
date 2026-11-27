// Categories: Platform > System Shutdown | Resources > Memory Management
#include "game/types.h"

extern s32 gGameSession __asm__("D_8009FA94");
extern s32 gScriptVmVars __asm__("D_8009FAD4");
extern s32 gVmCountdowns __asm__("D_8009F9A4");
extern s32 gOpcodeTable __asm__("D_8009F9F8");

extern void gShutdownCvkSystem(void) __asm__("func_80023478");
extern void gHeapFreeRaw(s32 block) __asm__("func_8001E2C0");
extern void gShutdownDeviceLayer(void) __asm__("func_8001E540");
extern void gShutdownHeap(void) __asm__("func_80017BA0");

/**
 * @brief Shuts the runtime down again, releasing everything func_80076EB0()
 *        allocated.
 *
 * The calls run in reverse order of the initialisation: func_80023478()
 * (close the 8-slot object table), func_8001E2C0() for the four blocks
 * D_8009FA94, D_8009FAD4, D_8009F9A4 and D_8009F9F8, func_8001E540() (device
 * teardown) and gShutdownHeap() (free the heap range table and its block).
 *
 * @return Nothing.
 *
 * @par Globals written
 * The four block pointers are passed to the deallocator but not cleared, so a
 * second call would double-free; gShutdownHeap() does clear D_8009FA9C and
 * D_8009F590 after freeing.
 *
 * @par Callers
 * func_80076A2C, after the main loop - which the retail code never leaves, so
 * this path is effectively dead in the shipping build.
 */
void gShutdownRuntime(void) __asm__("func_80076FE0");

void gShutdownRuntime(void)
{
    gShutdownCvkSystem();
    gHeapFreeRaw(gGameSession);
    gHeapFreeRaw(gScriptVmVars);
    gHeapFreeRaw(gVmCountdowns);
    gHeapFreeRaw(gOpcodeTable);
    gShutdownDeviceLayer();
    gShutdownHeap();
}
