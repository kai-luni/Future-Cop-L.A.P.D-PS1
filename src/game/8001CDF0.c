// Categories: Resources > Memory Management | Platform > CD-ROM
extern void gFreeBlock(void *block) __asm__("func_800181D4");

int gFreeDefaultCdBuffer(void *block) __asm__("func_8001CDF0");

/**
 * @brief Frees a buffer through the default CD file deallocator callback.
 *
 * @param block Buffer to release.
 * @return Always zero.
 *
 * @par Confirmed mechanics
 * Calls gFreeBlock with the incoming pointer in $a0, then clears $v0 before
 * returning.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * The file layer installs the address alongside gAllocateDefaultCdBuffer when
 * no custom free callback is supplied.
 *
 * @par Unknown
 * The direct listing does not establish which release requests take this
 * fallback.
 */
int gFreeDefaultCdBuffer(void *block)
{
    gFreeBlock(block);
    return 0;
}
