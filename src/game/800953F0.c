// Categories: Platform > GPU | Graphics > GPU > Debug Validation
#include "game/dma_globals.h"

#include "game/gpu_globals.h"

__asm__(".section .text.target,\"ax\",@progbits\n\t.balign 4");
typedef unsigned int u32;

typedef struct Entry {
    u32 data;
    short width;
    short height;
} Entry;

typedef struct Callbacks {
    void *pad[8];
    void (*write)(void *, void *);
} Callbacks;

extern char D_80012B7C[];
extern volatile u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 gPendingDeadline __asm__("D_8009DF00");
extern u32 gTimeoutStamp __asm__("D_8009DEFC");
extern Callbacks *gVramWriteCallback __asm__("D_8009DDE8");
extern int func_80091270(int);
extern void func_80092CF4(char *, void *);
extern int func_8009520C(void);
extern void func_800914E0(int, void (*)(void));
extern void gAbortCallbackId(void) __asm__("D_80095808");

/**
 * @brief Debug device write with a GPU-ready timeout (variant A).
 *
 * @param entry Debug entry {data, width, height}.
 * @param data Payload forwarded to the device write callback.
 * @return 0 on success, -1 when the abort check func_8009520C fires.
 *
 * @par Confirmed
 * func_80092CF4(D_80012B7C, entry) formats the entry; the deadline
 * D_8009DEFC = func_80091270(-1) + 0xF0 (frame wait plus 240), D_8009DF00
 * is cleared. The code polls the GPU status words D_8009DED4 (bit
 * 0x01000000) and D_8009DEC8 (bit 0x04000000) until both are ready, with
 * func_8009520C() as the abort path. Then func_800914E0(2, D_80095808)
 * installs the display callback and D_8009DDE8->write(entry, data) sends
 * the entry.
 *
 * @par Likely role
 * One of the two debug capture paths (twin func_800954DC uses a different
 * format string): the GPU must be idle before the debug device samples the
 * frame.
 *
 * @par Unknown
 * What device D_8009DDE8 addresses and what D_80095808 does on VBlank.
 *
 * @par Callers
 * func_800824CC.
 */
int func_800953F0(Entry *entry, void *data)
{
    int result;
    register char *first __asm__("$4");
    __asm__("lui %0,%%hi(D_80012B7C)\n\taddiu %0,%0,%%lo(D_80012B7C)" : "=r"(first));
    func_80092CF4(first, entry);
    gTimeoutStamp = func_80091270(-1) + 0xF0;
    gPendingDeadline = 0;
    if (!((*gDma2ChannelCtrlPtr & 0x01000000) || !(*gGpuGp1PortPtr & 0x04000000))) goto success;
loop:
    if (func_8009520C()) { result = -1; goto done; }
check:
    if (*gDma2ChannelCtrlPtr & 0x01000000) goto loop;
    if (!(*gGpuGp1PortPtr & 0x04000000)) goto loop;
success:
    {
        register void (*callback)(void) __asm__("$5");
        __asm__("lui %0,%%hi(D_80095808)\n\taddiu %0,%0,%%lo(D_80095808)" : "=r"(callback));
        func_800914E0(2, callback);
    }
    gVramWriteCallback->write(entry, data);
    result = 0;
done:
    __asm__ volatile(".set reorder");
    return result;
}
