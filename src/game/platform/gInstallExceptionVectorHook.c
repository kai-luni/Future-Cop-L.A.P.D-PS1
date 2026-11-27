// Categories: Platform > Interrupts | BIOS > Exception Vector
typedef unsigned int u32;

extern u32 gExceptionVectorBackup[] __asm__("D_80076A0C");
extern void gLoadKind57SpawnRecord(void) __asm__("func_80076878");
extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
void gInstallExceptionVectorHook(void) __asm__("func_80077340");


/**
 * @brief Saves the MIPS general exception vector and installs a custom trampoline.
 *
 * Disables interrupts, backs up 8 words (32 bytes) from address 0x80 into D_80076A0C,
 * patches 0x80..0x8C with a lui/ori/jr $k0/nop jump to func_80076878+0x4C, and re-enables interrupts.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_80076A0C - 8-word backup buffer holding the original 0x80 exception vector (restored by func_800772E0).
 *
 * @par Control flow
 * Enters critical section, copies 8 words from 0x80 to D_80076A0C, writes 4 instruction words at 0x80..0x8C, exits critical section.
 *
 * @par Calls
 * func_8008C9C0 - disables interrupts via COP0 status; func_8008C9E0 - enables interrupts via COP0 status.
 *
 * @par Side effects
 * Overwrites D_80076A0C, patches 16 bytes at RAM address 0x80 (MIPS exception vector), and modifies COP0 status.
 *
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s. Unknown: whether invoked dynamically or left unused beside func_800772E0.
 */
void gInstallExceptionVectorHook(void)
{
    u32 *src = (u32 *)0x80;
    u32 *dst = gExceptionVectorBackup;
    register u32 handler __asm__("$3");
    register u32 instruction __asm__("$2");
    register u32 first __asm__("$4");
    register u32 second __asm__("$4");
    int i;

    gDisableCpuInterrupts();
    for (i = 0; i < 8; i++) {
        *dst++ = *src++;
    }
    handler = (u32)gLoadKind57SpawnRecord + 0x4c;
    __asm__ volatile ("" : "+r" (handler));
    first = handler >> 16;
    __asm__ volatile ("" : "+r" (first));
    first |= 0x3c1a0000;
    handler &= 0xffff;
    __asm__ volatile ("" : "+r" (handler));
    instruction = 0x375a0000;
    __asm__ volatile ("" : "+r" (instruction));
    ((u32 *)0x80)[0] = first;
    second = handler | instruction;
    __asm__ volatile ("" : "+r" (second));
    __asm__ volatile ("" : : : "memory");
    instruction = 0x03400008;
    __asm__ volatile ("" : "+r" (instruction));
    ((u32 *)0x80)[1] = second;
    ((u32 *)0x80)[2] = instruction;
    ((u32 *)0x80)[3] = 0;
    gEnableCpuInterrupts();
}
