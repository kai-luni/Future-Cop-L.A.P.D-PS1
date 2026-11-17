// Categories: Platform > Sound | Platform > DMA | Platform > Interrupts
#include "game/types.h"

typedef void (*Callback)(void);
extern s32 gSecondCallGuard __asm__("D_8009EE94");
extern void *gSpuDmaCallback __asm__("D_8009EA2C");
extern void func_800972F8(void);
extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_800979F8(Callback callback);
extern void *func_8008C960(u32, s32, s32, s32);
extern void func_8008C980(void *);

/**
 * @brief One-time SPU DMA event init: registers the transfer callback and
 *        enables the completion event inside a critical section.
 *
 * @return Nothing; a second call is a no-op (D_8009EE94 guard).
 *
 * @par Confirmed
 * Under the D_8009EE94 == 0 guard: EnterCriticalSection (syscall 1 via
 * func_8008C9A0), func_800979F8(func_800972F8) installs the DMA callback,
 * OpenEvent(0xF0000009, 0x20, 0x2000, 0) (BIOS B0:08 via func_8008C960)
 * creates the event and its handle is stored in D_8009EA2C, EnableEvent
 * (B0:0C via func_8008C980) arms it, ExitCriticalSection (syscall 2 via
 * func_8008C9B0) closes the section. The guard word is set to 1 in the
 * same delay slot as the first critical-section entry.
 *
 * @par Likely role
 * The sound DMA interrupt setup: class 0xF0000009 is the SPU transfer
 * completion event, mode 0x2000 is the interrupt-driven event mode, and
 * func_800972F8 is the callback that advances the transfer queue.
 *
 * @par Unknown
 * The meaning of event spec 0x20.
 *
 * @par Callers
 * func_80021B50, func_80096D58.
 */
void func_80096E40(void)
{
    if (gSecondCallGuard == 0) {
        __asm__ volatile(".set\tnoreorder\n\t"
                         "addiu\t$v0,$zero,1\n\t"
                         "lui\t$at,%%hi(D_8009EE94)\n\t"
                         "jal\tfunc_8008C9A0\n\t"
                         "sw\t$v0,%%lo(D_8009EE94)($at)\n\t"
                         ".set\treorder" ::: "memory");
        func_800979F8(func_800972F8);
        func_8008C960(0xF0000009, 0x20, 0x2000, 0);
        __asm__ volatile(".set\tnoreorder\n\t"
                         "addu\t$a0,$v0,$zero\n\t"
                         "lui\t$at,%%hi(D_8009EA2C)\n\t"
                         "jal\tfunc_8008C980\n\t"
                         "sw\t$a0,%%lo(D_8009EA2C)($at)\n\t"
                         ".set\treorder" ::: "memory");
        func_8008C9B0();
    }
}
