// Categories: Game Engine > State Access
extern int *D_8009DEC8;

/**
 *  @brief Returns the integer referenced by a global pointer.
 *
 *  Reads the current value through D_8009DEC8.
 *  The source does not identify the value's purpose.
 *  @return The integer at the pointed-to address.
 *  @par Globals
 *  D_8009DEC8 supplies the address to read.
 *  @par Control flow
 *  Dereferences the pointer and returns without validation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None beyond the memory read.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
int func_80094144(void)
{
    return *D_8009DEC8;
}
