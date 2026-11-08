#include "game/types.h"

typedef struct {
    u8 pad00[0x1A6];
    u16 field1A6;
    volatile u16 field1A8;
    u16 field1AA;
    u8 pad1AC[2];
    volatile u16 field1AE;
} SpuDebug;

extern SpuDebug *D_8009EEA4;
extern u16 D_8009EEBC;
extern char D_80012CDC[];
extern char D_80012CFC[];
extern char D_80012D10[];

extern void func_80097990(void);
extern void func_8008E000(char *arg0, char *arg1);

void func_80097138(u16 *arg0, u32 arg1)
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

    want = D_8009EEA4->field1AE & 0x7FF;
    D_8009EEA4->field1A6 = D_8009EEBC;
    func_80097990();

    while (rem != 0) {
        count = 0x40;
        if (rem < 0x41) {
            count = rem;
        }
        i = 0;
        if (count > 0) {
            do {
                D_8009EEA4->field1A8 = *src++;
                i += 2;
            } while ((s32)i < count);
        }
        ptr = D_8009EEA4;
        ctl = ptr->field1AA;
        __asm__ volatile ("" : "+r" (ctl), "+r" (ptr));
        mask = (ctl & 0xFFCF) | 0x10;
        __asm__ volatile ("" : "+r" (mask));
        ptr->field1AA = mask;
        func_80097990();
        i = 0;
        while (D_8009EEA4->field1AE & 0x400) {
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
        func_80097990();
        func_80097990();
    }

    ptr2 = D_8009EEA4;
    ctl = *(volatile u16 *)&ptr2->field1AA;
    __asm__ volatile ("" : "+r" (ctl), "+r" (want));
    cmp = (u16)want;
    __asm__ volatile ("" : "+r" (cmp));
    mask2 = ctl & 0xFFCF;
    *(volatile u16 *)&ptr2->field1AA = mask2;
    k = 0;
    while ((D_8009EEA4->field1AE & 0x7FF) != cmp) {
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
