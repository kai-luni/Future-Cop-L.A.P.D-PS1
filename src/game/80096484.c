// Categories: Platform > DMA | Graphics > Image Transfer
extern int *D_8009E9F4;

/**
 * @brief Reads the transfer status word that D_8009E9F4 points at.
 *
 * D_8009E9F4 is a RAM pointer to the 32-bit control/status word of the image-transfer engine, not the register itself; the engine's command routines (func_80096150, func_8009649C) drive the same pointer with the sequence 0x80000000, 0, 0x60000000.
 *
 * @return The current content of the pointed-to word.
 *
 * @par Globals
 * D_8009E9F4 - pointer to the status word; dereferenced, never modified.
 *
 * @par Control flow
 * Two loads, no branches and no loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed; the read does not disturb the register block.
 *
 * @par Callers
 * Confirmed: func_80096090 (0x800960AC), which takes the result apart as (word >> 29) & 1 for its non-blocking status query, and func_800960CC (0x800960E8).
 */
int func_80096484(void)
{
    return *D_8009E9F4;
}
