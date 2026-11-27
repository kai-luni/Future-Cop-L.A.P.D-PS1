int gGetMcStateSize(void) __asm__("func_80020610");

// Categories: Platform > Memory Card | Utilities > Constants
/**
 * @brief Returns the size of the memory-card manager block.
 *
 * @return 0x1BB8, always - the byte size that func_8007F9CC() allocates and that
 *         func_80020618() and func_800206D0() work on: 16 slot states, 16 name
 *         buffers, 16 directory frames, 8 BIOS event handles, the command and
 *         result words and a 0x1200-byte format image.
 *
 * @par Callers
 * func_8007F9CC (allocation size) and func_80020618 (clearing the block).
 *
 * @par Role and path
 * Returns the constant byte size 0x1BB8 of the memory-card manager state
 * structure. Touches no globals and makes no calls; called by func_8007F9CC
 * for allocation and func_80020618 for zeroing.
 */
int gGetMcStateSize(void)
{
    return 0x1BB8;
}
