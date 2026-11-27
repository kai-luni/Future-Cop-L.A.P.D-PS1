// Categories: Platform > Assembly
/* Handwritten function */
/**
 *  @brief Defines a handwritten assembly entry point using two raw words.
 *
 *  The source emits 0x19097350 and 0x004162B3 with instruction reordering disabled.
 *  Its intended role and complete runtime behavior are not established by the source.
 *  @par Globals
 *  No D_800xxxxx globals are named in the source.
 *  @par Control flow
 *  The first word encodes a conditional branch; its target is not shown.
 *  @par Calls
 *  No direct callees are listed.
 *  @par Side effects
 *  Not established by the source.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80095870\n"
    ".ent func_80095870\n"
    "func_80095870:\n"
    ".word 0x19097350\n"
    ".word 0x004162B3\n"
    ".end func_80095870\n"
    ".set reorder\n"
);
