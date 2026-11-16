// Categories: Platform > Hardware Control
#include "game/spu_globals.h"

#include "game/types.h"

struct Hardware {
    u8 pad[0x1A6];
    u16 field1A6;
    u16 field1A8;
    u16 field1AA;
};

extern volatile struct Hardware *gSpuRegBasePtr __asm__("D_8009EEA4");
extern u32 *D_8009EEA8;
extern u32 *D_8009EEAC;
extern u32 *D_8009EEB0;
extern u32 D_8009EEF4;
extern void func_80097990(void);
extern void func_80097968(void);

/**
 *  @brief Configures hardware fields and starts a control operation.
 *
 *  Writes a 16-bit hardware field, sets control bits, and programs three
 *  global pointer targets with an address, a shifted value, and a control word.
 *  @param arg0 Value written to the first control target.
 *  @param arg1 Value truncated to the hardware field at offset 0x1A6.
 *  @param arg2 Value shifted left 16 bits and combined with 0x10.
 *  @par Globals
 *  D_8009EEA4 points to hardware fields; D_8009EEA8, D_8009EEAC, and
 *  D_8009EEB0 point to control targets; D_8009EEF4 is set to one.
 *  @par Control flow
 *  Performs the writes and helper calls in a fixed sequence without branches.
 *  @par Calls
 *  func_80097990 is called twice around the field update and shift;
 *  func_80097968 is called before the control targets are written.
 *  @par Side effects
 *  Modifies hardware fields, control targets, and D_8009EEF4.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800973B4(u32 arg0, u32 arg1, u32 arg2)
{
    register u32 control __asm__("$4");
    gSpuRegBasePtr->field1A6 = arg1;
    func_80097990();
    gSpuRegBasePtr->field1AA |= 0x30;
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
