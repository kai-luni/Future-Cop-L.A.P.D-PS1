// Categories: Resources > Memory Management | Platform > CD-ROM
extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Default memory deallocator wrapper registered by the file/CD layer.
 *
 * Releases block via func_800181D4 and always returns 0. Installed into D_8009F22C by func_8001D248 as the default free callback alongside allocator func_8001CDD0 when custom callbacks are omitted.
 *
 * @param block Pointer previously obtained from func_80017EBC / func_8001CDD0.
 * @return Always 0.
 *
 * @par Globals
 * None directly; address is stored in D_8009F22C by func_8001D248.
 *
 * @par Control flow
 * Single call to func_800181D4(block), followed by returning 0.
 *
 * @par Calls
 * func_800181D4 - releases the block back to its pool, free-range table, or general allocator.
 *
 * @par Side effects
 * Frees block via func_800181D4.
 *
 * @par Callers
 * Confirmed: none direct; address taken in func_8001D248 (0x8001D298) into D_8009F22C / Likely: file-layer buffer cleanup via D_8009F22C / Unknown: no indirect call site of D_8009F22C observed in asm/us/main.s.
 */
int func_8001CDF0(void *block)
{
    gFreeBlock(block);
    return 0;
}
