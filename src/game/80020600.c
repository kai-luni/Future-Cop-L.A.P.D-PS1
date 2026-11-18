// Categories: Resources > Save Data | Platform > Memory Card
static int *const value = (int *)0x8009AB08;

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
 */
void func_80020600(void)
{
    *value = 0;
}
