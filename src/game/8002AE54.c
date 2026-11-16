// Categories: Game Engine > Dispatch
#include "game/types.h"

extern u8 D_80039C58[];
extern u8 D_800357C0[];
extern u8 D_80041B6C[];
extern u8 D_8002DEA8[];
extern u8 D_80041C6C[];
extern u8 D_8002DB9C[];
extern u8 D_8003015C[];
extern u8 D_80030960[];
extern u8 D_8002F7C8[];
extern u8 D_8002ED7C[];
extern u8 D_8002C3F0[];
extern u8 D_80030D3C[];
extern u8 D_8003A844[];
extern u8 D_80042330[];
extern u8 D_800426A8[];
extern u8 D_8003151C[];

extern void func_80032F3C(void);
extern void func_8005908C(void);
extern void func_800382B4(void);
extern void func_8002D664(void);
extern void func_800395A8(void);
extern void func_800398EC(void);

/**
 *  @brief Maps a type number to a stored address.
 *
 *  The switch returns addresses of declared data symbols or functions as signed
 *  integers, allowing the caller to select an address by type.
 *  @param type Numeric selector for the address mapping.
 *  @return The selected address cast to s32, or zero for an unrecognized type.
 *  @par Globals
 *  Returns addresses of D_80039C58, D_800357C0, D_80041B6C, D_8002DEA8,
 *  D_80041C6C, D_8002DB9C, D_8003015C, D_80030960, D_8002F7C8,
 *  D_8002ED7C, D_8002C3F0, D_80030D3C, D_8003A844, D_80042330,
 *  D_800426A8, or D_8003151C for their corresponding selectors.
 *  @par Control flow
 *  A switch dispatches the selector; several cases share an address.
 *  @par Calls
 *  None; function addresses are returned without invoking them.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80060E74.
 */
s32 func_8002AE54(u32 type)
{
    switch (type) {
    case 80:
        return (s32)D_80039C58;
    case 9:
        return (s32)func_80032F3C;
    case 37:
        return (s32)D_800357C0;
    case 1:
        return (s32)func_8005908C;
    case 5:
        return (s32)func_800382B4;
    case 6:
    case 8:
    case 30:
    case 33:
    case 36:
        return (s32)func_8002D664;
    case 10:
        return (s32)D_80041B6C;
    case 11:
        return (s32)D_8002DEA8;
    case 12:
        return (s32)D_80041C6C;
    case 13:
        return (s32)D_8002DB9C;
    case 16:
        return (s32)D_8003015C;
    case 32:
        return (s32)D_80030960;
    case 20:
        return (s32)func_800395A8;
    case 25:
        return (s32)D_8002F7C8;
    case 27:
        return (s32)D_8002ED7C;
    case 26:
        return (s32)D_8002C3F0;
    case 28:
        return (s32)func_800398EC;
    case 29:
        return (s32)D_80030D3C;
    case 31:
        return (s32)D_8003A844;
    case 34:
        return (s32)D_80042330;
    case 35:
        return (s32)D_800426A8;
    case 14:
    case 38:
        return (s32)D_8003151C;
    default:
        return 0;
    }
}
