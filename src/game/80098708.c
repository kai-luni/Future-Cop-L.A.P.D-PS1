#include "game/types.h"

struct Entry { u32 value; u32 other; };
extern s32 D_8009EF04;
extern struct Entry *D_8009EF0C;
extern void func_80097DF8(void);

void func_80098708(u32 value)
{
    s32 i;
    register s32 count __asm__("$7");
    register s32 initial __asm__("$3") = D_8009EF04;
    struct Entry *entry;
    register u32 mask __asm__("$9");
    register u32 high __asm__("$2");
    register u32 tagged __asm__("$8");
    i = 0;
    if (initial > 0) {
    mask = 0x40000000;
    __asm__ volatile ("" : "=r" (mask) : "0" (mask));
    high = 0x80000000;
    __asm__ volatile ("" : "=r" (high) : "0" (high));
    tagged = value | high;
    __asm__ volatile ("" : : "r" (mask), "r" (tagged));
    count = initial;
    __asm__ volatile ("" : "=r" (count) : "0" (count));
    entry = D_8009EF0C;
    __asm__ volatile ("nop");
    goto body;
    for (; i < count; i++, entry++) {
body:
        if (entry->value & mask) break;
        if (entry->value == value) {
            entry->value = tagged;
            break;
        }
    }
    }
    func_80097DF8();
}
