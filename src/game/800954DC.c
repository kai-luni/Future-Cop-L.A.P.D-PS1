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
    void *pad[7];
    void (*write)(void *, void *);
} Callbacks;

extern char gDebugTagArg[] __asm__("D_80012AA0");
extern volatile u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 gPendingDeadline __asm__("D_8009DF00");
extern u32 gTimeoutStamp __asm__("D_8009DEFC");
extern Callbacks *gVramWriteCallback __asm__("D_8009DDE8");
extern int gVSyncWait(int) __asm__("func_80091270");
extern void gValidateGpuRectDebug(char *, void *) __asm__("func_80092CF4");
extern int func_8009520C(void);
extern void gSetDmaCallback(int, void (*)(void)) __asm__("func_800914E0");
extern void gAbortCallbackId(void) __asm__("D_80095808");

/**
 * @brief Debug device write with a GPU-ready timeout (variant B, while-loop
 *        form of func_800953F0).
 *
 * @param entry Debug entry {data, width, height}.
 * @param data Payload forwarded to the device write callback.
 * @return 0 on success, -1 on abort.
 *
 * @par Confirmed
 * Same sequence as func_800953F0 but with format string D_80012AA0 and a
 * while-loop over the two GPU status bits instead of the goto form; the
 * deadline, callback install and device write are identical.
 *
 * @par Likely role
 * The second debug capture variant; the different format string suggests a
 * different entry layout (likely the raw vs the annotated capture).
 *
 * @par Unknown
 * Which build configuration selects this variant.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; likely an unused retail variant.
 */
int func_800954DC(Entry *entry, void *data)
{
    int result;
    register char *first __asm__("$4");
    __asm__("lui %0,%%hi(D_80012AA0)\n\taddiu %0,%0,%%lo(D_80012AA0)" : "=r"(first));
    gValidateGpuRectDebug(first, entry);
    gTimeoutStamp = gVSyncWait(-1) + 0xF0;
    gPendingDeadline = 0;
    while ((*gDma2ChannelCtrlPtr & 0x01000000) || !(*gGpuGp1PortPtr & 0x04000000)) {
        if (func_8009520C()) { result = -1; goto done; }
    }
    {
        register void (*callback)(void) __asm__("$5");
        __asm__("lui %0,%%hi(D_80095808)\n\taddiu %0,%0,%%lo(D_80095808)" : "=r"(callback));
        gSetDmaCallback(2, callback);
    }
    gVramWriteCallback->write(entry, data);
    result = 0;
done:
    __asm__ volatile(".set reorder");
    return result;
}
