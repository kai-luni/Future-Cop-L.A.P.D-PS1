// Categories: BIOS > System Calls | Utilities > Memory
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Invokes PlayStation BIOS A0 vector 0x2B (memset) to fill a memory region with a byte value.
 *
 * Confirmed: handwritten assembly stub loads $t2 = 0xA0, jumps to $t2 with $t1 = 0x2B in the delay slot, passing caller arguments ($a0 dst, $a1 byte fill, $a2 size) straight to the PS1 BIOS A0(2Bh) table dispatch.
 * Category follows PS1 BIOS A0 vector 0x2B (memset) and caller usage zero-filling newly allocated heap pools/buffers.
 * Likely: standard PSX libc/BIOS memset thunk; Unknown: none.
 *
 * @param dst Destination buffer pointer in $a0 (implicit BIOS arg).
 * @param fill Fill byte value in $a1 (0 at all observed call sites).
 * @param size Byte count in $a2.
 * @return Return value of BIOS A0(0x2B) in $v0 (typically dst).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Three-instruction tail jump to BIOS A0 dispatcher (0xA0) with function index 0x2B in $t1.
 * @par Calls
 * PS1 BIOS A0(0x2B) (memset) via indirect jump jr $t2.
 * @par Side effects
 * Overwrites size bytes starting at dst via BIOS memset.
 * @par Callers
 * Confirmed: func_8004AB4C (0x8004AB78, zeroes 0x44C0-byte node pool); func_80063428 (0x80063484 zeroes D_8009F808, 0x800634B0 zeroes D_8009FA08) / Likely: none / Unknown: none.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CD20\n"
    ".ent func_8008CD20\n"
    "func_8008CD20:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2B\n"
    ".end func_8008CD20\n"
    ".set reorder\n"
);
