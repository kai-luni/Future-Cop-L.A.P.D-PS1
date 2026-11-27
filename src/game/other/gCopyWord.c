void gCopyWord(int *destination, int *source) __asm__("func_8001C27C");

// Categories: Utilities > Memory > Copy
/**
 * @brief Copies a single word from source to destination.
 *
 * @param destination Target word pointer.
 * @param source Source word pointer.
 * @return Nothing.
 *
 * @par Confirmed
 * *destination = *source; nothing else.
 *
 * @par Likely role
 * A one-word move helper of the draw/DMA setup family (func_8001C680,
 * func_8001C718, func_8001CE54): likely a register or pointer slot copy
 * the compiler kept as a call for alignment or ordering reasons.
 *
 * @par Callers
 * func_8001C680, func_8001C718, func_8001CE54.
 *
 * @par Role and path
 * Copies a single 32-bit integer word from *source to *destination with no
 * global accesses or outgoing calls. Called by func_8001C680, func_8001C718,
 * and func_8001CE54 to copy CdlLOC words between gCdTargetLoc and
 * gCdCurrentLoc.
 */
void gCopyWord(int *destination, int *source)
{
    *destination = *source;
}
