// Categories: Platform > CD-ROM | Resources > File Access
typedef unsigned int u32;
typedef int s32;
struct State {
    s32 count;
    void * volatile buffer;
    s32 unk8;
    s32 mode;
    s32 command;
    s32 unk14;
    s32 unk18;
    u32 time;
    s32 unk20;
    volatile s32 busy;
    s32 callback;
    s32 callback2;
    s32 callback3;
};
extern struct State gCdAsyncReadState __asm__("D_8009CBD0");
extern volatile s32 gCdReadBusyPtr[] __asm__("D_8009CBF4");
extern s32 gCdSavedCallbackPtr[] __asm__("D_8009CBF8");
extern s32 gCdUseDmaFlag __asm__("D_8009C814");
extern u32 gVSyncWait(s32) __asm__("func_80091270");
extern s32 gSetCdSyncCallback(s32) __asm__("func_8008E194");
extern s32 gSetCdReadyCallback(s32) __asm__("func_8008E1A8");
extern s32 gSetCdDmaCallback(s32) __asm__("func_8008E5D8");
extern s32 gGetCdStatusByte(void) __asm__("func_8008E010");
extern void gCdControlB(s32, s32, s32) __asm__("func_8008E42C");
extern s32 gStartCdSectorRead(s32) __asm__("func_80090C18");
s32 gCdReadAsync(s32 count, void *buffer, s32 mode) __asm__("func_80090FA0");


/**
 * @brief Configures CD-ROM sector read parameters and starts an asynchronous read.
 *
 * Waits up to 121 ticks for D_8009CBF4 (D_8009CBD0.busy) to clear, selects sector
 * word size from (mode & 0x30) (0x200, 0x249, or 0x246) with double-speed bit 0x20,
 * saves/clears CD and DMA3 callbacks, pauses any active drive motion, and calls func_80090C18.
 *
 * @param count Number of sectors to read into buffer.
 * @param buffer Destination RAM buffer for read sectors.
 * @param mode CD mode bits (bits 4..5 select sector size; bit 5 = 0x20 forced on).
 * @return 1 if func_80090C18(0) returned > 0 (read armed), else 0.
 *
 * @par Globals
 * D_8009CBD0 - CD read state; D_8009CBF4/D_8009CBF8 - aliases for busy/callback; D_8009C814 - mode flag bit 0.
 *
 * @par Control flow
 * Spins up to 121 ticks on *D_8009CBF4 restoring *D_8009CBF8 on timeout, switches on mode & 0x30, pauses CD if status & 0xE0, then calls func_80090C18(0).
 *
 * @par Calls
 * func_80091270 - tick clock; func_8008E194/E1A8/E5D8 - save/clear hooks; func_8008E010/E42C - status check and CdlPause (9); func_80090C18 - start CD read.
 *
 * @par Side effects
 * Mutates D_8009CBD0, swaps CD/DMA3 callbacks, may issue CdlPause, and initiates CD read.
 *
 * @par Callers
 * Confirmed: func_8009081C (0x80090858, blocking CD sector reader).
 *
 * @par Role and path
 * Waits up to 121 ticks for *D_8009CBF4 (D_8009CBD0.busy) to clear,
 * configures D_8009CBD0 with count, buffer, sector word size (0x200, 0x249,
 * or 0x246 from mode & 0x30), and double-speed bit 0x20, and saves the prior
 * CD/DMA3 callbacks. It pauses the drive via func_8008E42C if func_8008E010()
 * & 0xE0 is non-zero, then calls func_80090C18(0) and returns 1 if > 0.
 * Driven by func_8009081C.
 */
s32 gCdReadAsync(s32 count, void *buffer, s32 mode)
{
    register s32 n __asm__("$20");
    register void *buf __asm__("$19");
    register s32 m __asm__("$18");
    register volatile s32 *busy __asm__("$16");
    register u32 start __asm__("$17");
    register struct State *state __asm__("$4");
    register struct State *other __asm__("$3");
    register s32 command __asm__("$2");
    register s32 z __asm__("$4");
    s32 mv;
    n = count;
    buf = buffer;
    busy = gCdReadBusyPtr;
    __asm__ volatile("" : "+r" (n), "+r" (buf), "+r" (busy));
    m = mode;
    if (*busy) {
        start = gVSyncWait(-1);
        while (*busy) {
            if (gVSyncWait(-1) - start >= 121) {
                busy = gCdSavedCallbackPtr;
                __asm__ volatile("" : "+r" (busy));
                gSetCdSyncCallback(*busy);
                busy -= 10;
                busy[9] = 0;
                goto ready;
            }
        }
    }
ready:
    __asm__ volatile("");
    state = &gCdAsyncReadState;
    __asm__ volatile("" : "+r" (state));
    state->mode = m;
    switch (((volatile struct State *)state)->mode & 0x30) {
    case 0:
        state->command = 0x200;
        break;
    case 0x20:
        state->command = 0x249;
        break;
    default:
        command = 0x246;
        __asm__ volatile("" : "+r" (command));
        other = &gCdAsyncReadState;
        __asm__ volatile("" : "+r" (other));
        other->command = command;
        break;
    }
    busy = (volatile s32 *)&gCdAsyncReadState;
    __asm__ volatile("" : "+r" (busy));
    mv = ((volatile struct State *)busy)->mode;
    __asm__ volatile("" : "+r"(mv));
    z = 0;
    __asm__ volatile("" : "+r"(z));
    ((volatile struct State *)busy)->mode = mv | 0x20;
    ((struct State *)busy)->buffer = buf;
    __asm__ volatile("" : "+r"(n));
    ((struct State *)busy)->count = n;
    ((struct State *)busy)->callback = gSetCdSyncCallback(z);
    ((struct State *)busy)->callback2 = gSetCdReadyCallback(0);
    {
        s32 flags = gCdUseDmaFlag;
        __asm__ volatile("" : "+r" (flags));
        if (flags & 1)
            ((struct State *)busy)->callback3 = gSetCdDmaCallback(0);
    }
    ((struct State *)busy)->time = gVSyncWait(-1);
    __asm__ volatile("" : : : "memory");
    if (gGetCdStatusByte() & 0xe0) {
        register s32 zero __asm__("$5");
        register s32 nine __asm__("$4");
        nine = 9;
        __asm__ volatile("" : "+r" (nine));
        zero = 0;
        __asm__ volatile("" : "+r" (zero));
        gCdControlB(nine, zero, 0);
    }
    {
        s32 result = gStartCdSectorRead(0);
        __asm__ volatile(".set reorder");
        return result > 0;
    }
}
