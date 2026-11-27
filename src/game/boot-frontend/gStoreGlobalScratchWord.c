// Categories: Game Engine > State
#include "game/types.h"
void gStoreGlobalScratchWord(u32 value) __asm__("func_8008FE1C");


/**
 *  @brief Stores a 32-bit value at a fixed address.
 *
 *  The function writes directly to address 0x8009CB50; the purpose of
 *  the stored value is not identified by this source.
 *  @param value Value to write.
 *  @par Globals
 *  Address 0x8009CB50 is written directly; no named global is declared.
 *  @par Control flow
 *  Performs one unconditional store.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the 32-bit word at 0x8009CB50.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Performs a single unconditional 32-bit store of value to the fixed address
 * 0x8009CB50. No callers or other references to 0x8009CB50 appear in src/, so
 * the subsystem and purpose of the stored word cannot be established from the
 * codebase.
 */
void gStoreGlobalScratchWord(u32 value)
{
    register u32 *p __asm__("$1");

    p = (u32 *)0x8009CB50;
    *p = value;
}
