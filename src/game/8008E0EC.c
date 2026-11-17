// Categories: Resources > Lookup
extern void *gIndexEntryTable[] __asm__("D_8009C8CC");

/**
 *  @brief Looks up a pointer by byte index with a fallback.
 *
 *  Indices below 28 select entries from D_8009C8CC; other indices
 *  return the fixed address 0x8001233C.
 *  @param index Unsigned byte index to look up.
 *  @return The selected table pointer or the fallback address.
 *  @par Globals
 *  D_8009C8CC is read for indices below 28.
 *  @par Control flow
 *  Branches on the index bound and returns one of two pointer sources.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void *func_8008E0EC(unsigned char index)
{
    if (index >= 28) {
        char *base = (char *)0x80010000;
        __asm__("" : "=r"(base) : "0"(base));
        return base + 0x233C;
    }
    return gIndexEntryTable[index];
}
