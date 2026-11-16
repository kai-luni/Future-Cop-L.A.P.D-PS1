// Categories: Graphics > Ordering Table | Platform > GPU
#include "game/gpu_globals.h"

__asm__(".section .text.target,\"ax\",@progbits\n\t.balign 4");
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct Callbacks {
    void *pad[6];
    void (*unk18)(s32);
} Callbacks;

extern u8 D_8009DDF2;
extern char D_80012AE8[];
extern void (*D_8009DDEC)(char *, s32);
extern volatile u32 *D_8009DED4;
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 D_8009DEFC, D_8009DF00;
extern Callbacks *D_8009DDE8;
extern int func_80091270(int);
extern int func_8009520C(void);
extern void func_800914E0(int, void (*)(void));
extern void D_80095808(void);

/**
 * @brief Waits for the GPU DMA path to go idle, then issues DrawOTag through the library callback block; returns 0 on success, -1 when the streaming watchdog trips.
 *
 * @param arg0 Ordering-table pointer forwarded to the draw callback (and to the debug print).
 * @return 0 after the draw was issued, -1 on a stalled transfer.
 *
 * @par Globals
 * D_8009DED4 - DMA2 channel control 0x1F8010A8, bit 24 = transfer busy. D_8009DEC8 - GPUSTAT 0x1F801814, bit 26 = ready for DMA. D_8009DEFC/D_8009DF00 - watchdog deadline (frame counter + 0xF0) and reset word. D_8009DDF2 - debug level; >= 2 prints "DrawOTag(%08x)..." through the D_8009DDEC hook. D_8009DDE8 - library callback block, unk18 is the draw entry.
 *
 * @par Control flow
 * Optional debug print, deadline setup, then a wait loop while DMA2 is busy or the GPU is not ready; each pass runs the watchdog, which returns -1 on a stall. On success the display callback is dispatched and the draw issued.
 *
 * @par Calls
 * func_80091270 - reads the frame counter (-1 = no wait) for the deadline. func_8009520C - streaming watchdog/stall reset. func_800914E0 - dispatches display callback slot +4; the literal args 2 and func_80095808 are ignored by the callee. D_8009DDE8->unk18 - the actual DrawOTag issue (indirect).
 *
 * @par Side effects
 * Sets the watchdog deadline words; draws the ordering table via the library callback.
 *
 * @par Callers
 * Confirmed: none - no call edge or address-taken reference in the listing; analysis marks the unit unreachable from entry. Unknown: likely unused library code; the caller named in the package work list does not exist in the unit catalog.
 */
int func_8009570C(s32 arg0)
{
    int result;
    if (D_8009DDF2 >= 2) {
        register char *msg __asm__("$4");
        __asm__("lui %0,%%hi(D_80012AE8)\n\taddiu %0,%0,%%lo(D_80012AE8)" : "=r"(msg));
        D_8009DDEC(msg, arg0);
    }
    D_8009DEFC = func_80091270(-1) + 0xF0;
    D_8009DF00 = 0;
    if (!((*D_8009DED4 & 0x01000000) || !(*gGpuGp1PortPtr & 0x04000000))) goto success;
loop:
    if (func_8009520C()) { result = -1; goto done; }
check:
    if (*D_8009DED4 & 0x01000000) goto loop;
    if (!(*gGpuGp1PortPtr & 0x04000000)) goto loop;
success:
    {
        register void (*callback)(void) __asm__("$5");
        __asm__("lui %0,%%hi(D_80095808)\n\taddiu %0,%0,%%lo(D_80095808)" : "=r"(callback));
        func_800914E0(2, callback);
    }
    D_8009DDE8->unk18(arg0);
    result = 0;
done:
    __asm__ volatile(".set reorder");
    return result;
}
