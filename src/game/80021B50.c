// Categories: Graphics > Sprite/Asset Subsystem | Resources > Memory Management
#include "game/types.h"

extern u8 gSubsystemTable[] __asm__("D_800A085C");

extern void func_80097A28(void);
extern void func_80096E40(void);
extern void func_80097AC8(s32 count, u8 *table);
extern void func_80021824(void);

/**
 * @brief Initialization chain of the sprite/asset subsystem: library
 *        warm start, callback setup, a 32-entry table and the state reset.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Fixed sequence: func_80097A28() (forwards to func_80096D58(0), the
 * warm-start form), func_80096E40(), func_80097AC8(0x20, D_800A085C)
 * (installs a 32-entry table), func_80021824().
 *
 * @par Likely role
 * Runs once when the subsystem comes up; the table at D_800A085C feeds
 * the buffer/slot management installed by func_80097AC8.
 *
 * @par Callers
 * func_8002333C.
 */
void func_80021B50(void)
{
    func_80097A28();
    func_80096E40();
    func_80097AC8(0x20, gSubsystemTable);
    func_80021824();
}
