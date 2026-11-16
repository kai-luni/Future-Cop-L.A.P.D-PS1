// Categories: Utilities > Stub
/**
 *  @brief Creates a local buffer referenced by an empty assembly statement.
 *
 *  The 16-byte buffer is passed as an input operand to volatile inline
 *  assembly, but the assembly contains no instructions.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  Executes the empty assembly statement without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  No externally visible state is changed by the source.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8004F08C(void)
{
    char b[16];
    __asm__ __volatile__("" : : "r"(b));
}
