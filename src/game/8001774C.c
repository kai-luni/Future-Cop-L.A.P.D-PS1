// Categories: Graphics > Scene Graph | Game Engine > Init
#include "game/types.h"

extern void *gCamera[] __asm__("D_8009F9E4");
extern void *gTransformRecordList __asm__("D_8009F588");
extern s32 gTransformSerialCounter __asm__("D_8009F58C");

/**
 * @brief Resets the camera pointer and transform-record list state at mission boot.
 *
 * Clears D_8009F9E4 (active camera pointer), D_8009F588 (0x94-byte transform-record list head freed by func_80017764), and D_8009F58C (serial ID counter incremented by func_80017994).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F9E4 - active camera block pointer, set to 0.
 * D_8009F588 - transform-record singly-linked list head (0x374($gp)), set to 0.
 * D_8009F58C - transform-record serial ID counter (0x378($gp)), set to 0.
 *
 * @par Control flow
 * Straight-line stores of zero to all three globals, then return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 0 to D_8009F9E4, D_8009F588, and D_8009F58C.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024C10, mission boot sequence) / Likely: none / Unknown: none.
 */
void func_8001774C(void)
{
    gCamera[0] = 0;
    gTransformRecordList = 0;
    gTransformSerialCounter = 0;
}
