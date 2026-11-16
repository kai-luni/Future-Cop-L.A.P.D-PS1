// Categories: Utilities > Bit Operations
/**
 *  @brief Sets the low 24 bits of an unsigned integer.
 *
 *  ORs the pointed-to value with 0x00FFFFFF.
 *  The upper eight bits retain their previous values.
 *  @param value Pointer to the unsigned integer to modify.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs one unconditional read-modify-write operation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the integer referenced by value.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void func_800923F8(unsigned int *value)
{
    *value |= 0x00FFFFFFU;
}
