// Categories: Resources > Memory Management
/**
 * @brief Returns the allocation size stored in the 4-byte header before a heap block.
 *
 * Reads value[-1] (offset -4 from the pointer returned by func_80017EBC), matching the header layout inspected by func_800181D4 when releasing blocks.
 *
 * @param value Pointer to an allocated heap block.
 * @return Signed 32-bit size word from value[-1].
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single memory load (lw $v0, -0x4($a0)) and immediate return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80063428 (0x80063474, 0x800634A0, zeroing terrain buffers) / Likely: none / Unknown: none.
 */
int func_80018460(int *value)
{
    return value[-1];
}
