// Categories: Game Engine > State
#include "game/gpu_globals.h"

extern unsigned int *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern unsigned char gGpuLibPair[] __asm__("D_800A62C8");

/**
 *  @brief Stores a word and records its low byte in a high-byte-indexed table.
 *
 *  Writes value through a global pointer, then uses its high byte as an index.
 *  The indexed byte receives the low eight bits of value.
 *  @param value Word to store and split into table index and byte value.
 *  @par Globals
 *  D_8009DEC8 supplies the destination pointer; D_800A62C8 is the byte table.
 *  @par Control flow
 *  Performs both writes unconditionally, using inline assembly to form the table address.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the pointed-to word and one byte of D_800A62C8.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80094928(unsigned int value)
{
    register unsigned char *base asm("$1");
    unsigned int index;
    *gGpuGp1PortPtr = value;
    index = value >> 24;
    asm volatile ("lui %0, %%hi(D_800A62C8)\n\taddu %0, %0, %1" : "=r" (base) : "r" (index));
    base[0x62c8] = value;
}
