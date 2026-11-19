// Categories: Resources > Save Data | Platform > Memory Card
static int *const value = (int *)0x8009AB08;
void gClearMcStatePtr(void) __asm__("func_80020600");


/**
 * @brief Clears the memory-card manager pointer.
 *
 * Stores 0 into the context pointer at 0x8009AB08 (declared as a constant
 * pointer, which is why the store compiles to a lui/sw pair). The block itself is
 * not freed here - the caller owns it (func_8007FA48 frees it right after). The
 * pointer belongs to the profile save/load subsystem, not to a decoder.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_8007FA48 only.
 *
 * @par Role and path
 * Clears the global memory-card manager pointer at 0x8009AB08 (D_8009AB08) to
 * 0 without freeing the underlying block. Writes D_8009AB08 and makes no
 * calls; called by func_8007FA48.
 */
void gClearMcStatePtr(void)
{
    *value = 0;
}
