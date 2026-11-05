#include "game/types.h"

struct Hardware {
    u8 pad[0x1A6];
    u16 field1A6;
    u16 field1A8;
    u16 field1AA;
};

extern volatile struct Hardware *D_8009EEA4;
extern u32 *D_8009EEA8;
extern u32 *D_8009EEAC;
extern u32 *D_8009EEB0;
extern u32 D_8009EEF4;
extern void func_80097990(void);
extern void func_80097968(void);

void func_800973B4(u32 arg0, u32 arg1, u32 arg2)
{
    register u32 control __asm__("$4");
    D_8009EEA4->field1A6 = arg1;
    func_80097990();
    D_8009EEA4->field1AA |= 0x30;
    __asm__ volatile("" : "+r" (arg2) : : "memory");
    arg2 <<= 16;
    func_80097990();
    __asm__ volatile("" : "+r" (arg2));
    func_80097968();
    control = 0x01000200;
    __asm__ volatile("" : "+r" (control));
    *D_8009EEA8 = arg0;
    __asm__ volatile("" : "+r" (arg2));
    *D_8009EEAC = arg2 | 0x10;
    D_8009EEF4 = 1;
    *D_8009EEB0 = control;
}
