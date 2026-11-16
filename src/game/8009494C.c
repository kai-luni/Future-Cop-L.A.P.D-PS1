// Categories: Utilities > Table Access
/**
 *  @brief Reads an indexed byte from a fixed memory address.
 *
 *  Uses address 0x800A62C8 as the base of the byte sequence.
 *  The source does not establish the sequence's purpose or bounds.
 *  @param index Byte offset from the base address.
 *  @return The selected unsigned byte, promoted to int.
 *  @par Globals
 *  No named global is referenced; memory at 0x800A62C8 is read.
 *  @par Control flow
 *  Computes the address and returns the byte without a bounds check.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None beyond the memory read.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
int func_8009494C(int index)
{
    register unsigned char *ptr asm("$2") = (unsigned char *)0x800A0000U;
    asm("" : "+r"(ptr));
    ptr += index;
    return ptr[0x62C8];
}
