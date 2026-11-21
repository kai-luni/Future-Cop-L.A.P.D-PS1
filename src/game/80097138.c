// Categories: Platform > SPU | Platform > Sound
#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    u8 pad00[0x1A6];
    u16 field1A6;
    volatile u16 field1A8;
    u16 field1AA;
    u8 pad1AC[2];
    volatile u16 field1AE;
} SpuDebug;

extern SpuDebug *gSpuRegBasePtr __asm__("D_8009EEA4");
extern u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern char gSpuTimeoutFormat[] __asm__("D_80012CDC");
extern char gSpuWriteReadyTimeoutText[] __asm__("D_80012CFC");
extern char gSpuWaitDmafClearStr[] __asm__("D_80012D10");

extern void gSpuBusyWait(void) __asm__("func_80097990");
extern void func_8008E000(char *arg0, char *arg1);

/**
 * @brief Uploads a block of bytes to PlayStation SPU Sound RAM via manual FIFO writes.
 *
 * Snapshots SPUSTAT & 0x7FF, programs SPU transfer address D_8009EEBC into D_8009EEA4->field1A6, transfers arg1 bytes in chunks of up to 0x40 bytes through FIFO field1A8 with SPUCNT transfer bits 0x10 (manual write) while polling SPUSTAT bit 0x400, then clears transfer bits and waits for SPUSTAT & 0x7FF to settle. Category Platform > SPU | Platform > Sound reflects direct SPU MMIO access (0x1F801C00 + 0x1A6..0x1AE) and SPU timeout logging.
 *
 * @param arg0 Pointer to source 16-bit halfwords to write into SPU RAM.
 * @param arg1 Number of bytes to transfer (written as halfwords, up to 0x40 bytes per chunk).
 *
 * @par Globals
 * D_8009EEA4 - SPU MMIO base; D_8009EEBC - target SPU RAM address; D_80012CDC/D_80012CFC/D_80012D10 - "SPU:T/O [%s]\n" and wait strings.
 *
 * @par Control flow
 * Outer chunk loop (<= 0x40 bytes) with inner halfword FIFO loop, 0xF00-capped wrdy wait loop, and final 0xF00-capped dmaf wait loop.
 *
 * @par Calls
 * gSpuBusyWait - hardware settling delay loop; func_8008E000 - logs SPU timeout diagnostic if wrdy or dmaf wait exceeds 0xF00 polls.
 *
 * @par Side effects
 * Writes SPU transfer address, FIFO, and control registers, uploading data into SPU Sound RAM.
 *
 * @par Callers
 * Confirmed: gSpuResetHardware (0x80097048), func_800976DC (0x80097740) / Likely: none / Unknown: none.
 */
void gSpuWriteMemBlock(u16 *arg0, u32 arg1) __asm__("func_80097138");

void gSpuWriteMemBlock(u16 *arg0, u32 arg1)
{
    register u32 rem __asm__("$17") = arg1;
    register u16 *src __asm__("$18") = arg0;
    u32 want;
    s32 count;
    register u32 i __asm__("$3");
    u32 k;
    register u32 ctl __asm__("$4");
    register char *a __asm__("$4");
    register char *b __asm__("$5");
    register u32 mask __asm__("$2");
    register SpuDebug *ptr __asm__("$3");
    register SpuDebug *ptr2 __asm__("$2");
    register u32 mask2 __asm__("$3");
    register u32 cmp __asm__("$5");

    want = gSpuRegBasePtr->field1AE & 0x7FF;
    gSpuRegBasePtr->field1A6 = gSpuTransferAddr;
    gSpuBusyWait();

    while (rem != 0) {
        count = 0x40;
        if (rem < 0x41) {
            count = rem;
        }
        i = 0;
        if (count > 0) {
            do {
                gSpuRegBasePtr->field1A8 = *src++;
                i += 2;
            } while ((s32)i < count);
        }
        ptr = gSpuRegBasePtr;
        ctl = ptr->field1AA;
        __asm__ volatile ("" : "+r" (ctl), "+r" (ptr));
        mask = (ctl & 0xFFCF) | 0x10;
        __asm__ volatile ("" : "+r" (mask));
        ptr->field1AA = mask;
        gSpuBusyWait();
        i = 0;
        while (gSpuRegBasePtr->field1AE & 0x400) {
            if (++i >= 0xF01) {
                __asm__ volatile ("" : : : "memory");
                a = (char *)0x80010000;
                __asm__ volatile ("" : "+r" (a));
                a += 0x2cdc;
                __asm__ volatile ("" : "+r" (a));
                b = (char *)0x80010000;
                __asm__ volatile ("" : "+r" (b));
                func_8008E000(a, b + 0x2cfc);
                break;
            }
        }
        rem -= count;
        gSpuBusyWait();
        gSpuBusyWait();
    }

    ptr2 = gSpuRegBasePtr;
    ctl = *(volatile u16 *)&ptr2->field1AA;
    __asm__ volatile ("" : "+r" (ctl), "+r" (want));
    cmp = (u16)want;
    __asm__ volatile ("" : "+r" (cmp));
    mask2 = ctl & 0xFFCF;
    *(volatile u16 *)&ptr2->field1AA = mask2;
    k = 0;
    while ((gSpuRegBasePtr->field1AE & 0x7FF) != cmp) {
        if (++k >= 0xF01) {
            __asm__ volatile ("" : : : "memory");
            a = (char *)0x80010000;
            __asm__ volatile ("" : "+r" (a));
            a += 0x2cdc;
            __asm__ volatile ("" : "+r" (a));
            b = (char *)0x80010000;
            __asm__ volatile ("" : "+r" (b));
            func_8008E000(a, b + 0x2d10);
            break;
        }
    }
    __asm__ volatile ("" : "=r" (k));
}
