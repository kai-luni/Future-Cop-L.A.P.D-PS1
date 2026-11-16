// Categories: Platform > Register Control
#include "game/dma_globals.h"

extern unsigned int D_8009DD64[];
extern unsigned int *gDmaInterruptCtrlPtr __asm__("D_8009DD60");

/**
 *  @brief Updates an indexed value and its corresponding control bit.
 *
 *  When the value changes, stores it in the indexed table and updates the
 *  pointed-to control word, setting bit 23 and the indexed bit as appropriate.
 *  @param arg0 Index into the value table and control-bit selection.
 *  @param arg1 New value for the indexed entry.
 *  @return The previous indexed value.
 *  @par Globals
 *  D_8009DD64 holds the indexed values; D_8009DD60 points to the control word.
 *  @par Control flow
 *  Does nothing if unchanged; otherwise sets or clears bit arg0 plus 16.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  On change, writes the table entry and control word, discarding the word's top byte.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
unsigned int func_80091E8C(int arg0, unsigned int arg1)
{
    register int index __asm__("$6") = arg0;
    register unsigned int value __asm__("$4");
    register unsigned int *base __asm__("$3");
    unsigned int old;
    register unsigned int result __asm__("$2");
    __asm__ volatile("" : "=r"(index) : "0"(index));
    value = arg1;
    base = D_8009DD64;
    old = base[index];
    result = old;
    if (value != old) {
        if (value) {
            register unsigned int tmp __asm__("$4");
            register int shift __asm__("$3");
            unsigned int bits;
            base[index] = value;
            tmp = *gDmaInterruptCtrlPtr & 0xFFFFFF;
            shift = index + 16;
            __asm__ volatile("" : "+r"(tmp), "+r"(shift));
            bits = (1 << shift) | 0x800000;
            __asm__ volatile("" : "+r"(bits));
            *gDmaInterruptCtrlPtr = tmp | bits;
            __asm__ volatile("" : : : "memory");
        } else {
            register unsigned int *ptr __asm__("$5") = gDmaInterruptCtrlPtr;
            register int shift __asm__("$4");
            register unsigned int mask __asm__("$2") = 0xFF0000;
            unsigned int tmp;
            __asm__ volatile("" : "+r"(value), "+r"(mask));
            __asm__ volatile("" : "+r"(ptr));
            mask |= 0xFFFF;
            base[index] = 0;
            tmp = (*ptr & mask) | 0x800000;
            shift = index + 16;
            __asm__ volatile("" : "+r"(ptr), "+r"(shift), "+r"(tmp));
            *ptr = tmp & ~(1 << shift);
        }
        result = old;
        __asm__ volatile("" : "+r"(result));
    }
    return result;
}
