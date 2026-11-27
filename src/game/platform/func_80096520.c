// Categories: Platform > Startup
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 *  @brief Initializes memory and registers before invoking startup routines.
 *
 *  Clears a global memory range, establishes stack and global-pointer registers,
 *  and computes a memory base and size from startup configuration values.
 *  @par Globals
 *  D_8009F528 starts the cleared range and D_800AB6D8 ends it; D_8009F390
 *  and D_8009F394 supply configuration values. D_8009EA04 and D_8009EA08
 *  receive the computed base and size; D_8009F990 saves the incoming return address.
 *  @par Control flow
 *  A word loop clears the range, then two routines are called in sequence;
 *  execution reaches a break instruction if the second call returns.
 *  @par Calls
 *  func_800966A0 receives the computed base plus four and the computed size;
 *  func_80076A2C runs after the saved return address is reloaded.
 *  @par Side effects
 *  Clears memory, writes startup globals, changes registers, and invokes helpers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
__asm__(
    ".set noreorder\n"
    ".globl entrypoint\n"
    ".ent entrypoint\n"
    "entrypoint:\n"
    ".word 0x3C02800A   /* lui        $v0, %hi(D_8009F528) */\n"
    ".word 0x2442F528   /* addiu      $v0, $v0, %lo(D_8009F528) */\n"
    ".word 0x3C03800B   /* lui        $v1, %hi(D_800AB6D8) */\n"
    ".word 0x2463B6D8   /* addiu      $v1, $v1, %lo(D_800AB6D8) */\n"
    ".word 0xAC400000   /* sw         $zero, 0x0($v0) */\n"
    ".word 0x24420004   /* addiu      $v0, $v0, 0x4 */\n"
    ".word 0x0043082B   /* sltu       $at, $v0, $v1 */\n"
    ".word 0x1420FFFC   /* bnez       $at, .L80096530 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x3C02800A   /* lui        $v0, %hi(D_8009F390) */\n"
    ".word 0x8C42F390   /* lw         $v0, %lo(D_8009F390)($v0) */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x2042FFF8   /* addi       $v0, $v0, -0x8 */\n"
    ".word 0x3C088000   /* lui        $t0, %hi(D_80000004) */\n"
    ".word 0x0048E825   /* or         $sp, $v0, $t0 */\n"
    ".word 0x3C04800B   /* lui        $a0, %hi(D_800AB6D8) */\n"
    ".word 0x2484B6D8   /* addiu      $a0, $a0, %lo(D_800AB6D8) */\n"
    ".word 0x000420C0   /* sll        $a0, $a0, 3 */\n"
    ".word 0x000420C2   /* srl        $a0, $a0, 3 */\n"
    ".word 0x3C03800A   /* lui        $v1, %hi(D_8009F394) */\n"
    ".word 0x8C63F394   /* lw         $v1, %lo(D_8009F394)($v1) */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00432823   /* subu       $a1, $v0, $v1 */\n"
    ".word 0x00A42823   /* subu       $a1, $a1, $a0 */\n"
    ".word 0x3C01800A   /* lui        $at, %hi(D_8009EA08) */\n"
    ".word 0xAC25EA08   /* sw         $a1, %lo(D_8009EA08)($at) */\n"
    ".word 0x00882025   /* or         $a0, $a0, $t0 */\n"
    ".word 0x3C01800A   /* lui        $at, %hi(D_8009EA04) */\n"
    ".word 0xAC24EA04   /* sw         $a0, %lo(D_8009EA04)($at) */\n"
    ".word 0x3C01800A   /* lui        $at, %hi(D_8009F990) */\n"
    ".word 0xAC3FF990   /* sw         $ra, %lo(D_8009F990)($at) */\n"
    ".word 0x3C1C800A   /* lui        $gp, (0x800A0000 >> 16) */\n"
    ".word 0x279CF214   /* addiu      $gp, $gp, -0xDEC */\n"
    ".word 0x03A0F021   /* addu       $fp, $sp, $zero */\n"
    ".word 0x0C0259A8   /* jal        func_800966A0 */\n"
    ".word 0x20840004   /* addi      $a0, $a0, %lo(D_80000004) */\n"
    ".word 0x3C1F800A   /* lui        $ra, %hi(D_8009F990) */\n"
    ".word 0x8FFFF990   /* lw         $ra, %lo(D_8009F990)($ra) */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x0C01DA8B   /* jal        func_80076A2C */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x0000004D   /* break      0, 1 */\n"
    ".end entrypoint\n"
    ".set reorder\n"
);
