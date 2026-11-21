// Categories: Platform > Fixed-address Dispatch
/* Tail jump to the fixed address 0xA000DFAC. A computed goto materialises
 * the address in $v0 and jumps without a call frame; naming the symbol
 * (link path) keeps the %hi/%lo pair as lui+addiu instead of lui+ori. */
extern char gKernelCardPatchEntryDFAC[] __asm__("D_A000DFAC");

/**
 *  @brief Transfers control to the fixed address 0xA000DFAC.
 *
 *  This wrapper uses the address of D_A000DFAC as a computed jump
 *  target, without creating a call frame. The target's behavior is
 *  not specified by the source.
 *  @par Globals
 *  D_A000DFAC supplies the fixed jump address; its contents are not read.
 *  @par Control flow
 *  Unconditionally jumps to the target without a local return path.
 *  @par Calls
 *  No named functions; control transfers to the fixed address.
 *  @par Side effects
 *  Effects after the transfer depend on the target code.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Transfers control unconditionally via a computed goto to the fixed address
 * D_A000DFAC (0xA000DFAC) without creating a call frame. No globals in
 * D_800xxxxx are accessed and no direct callers exist in src/.
 */
void func_8008DCC0(void)
{
    goto *(void *)gKernelCardPatchEntryDFAC;
}
