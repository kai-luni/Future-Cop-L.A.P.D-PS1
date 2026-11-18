// Categories: Resources > Save Data | Game Engine > State Machine
#include "game/types.h"

typedef struct {
    u8 pad0[0x200];
    u8 unk200;
    u8 pad201[2];
    u8 unk203;
    u8 pad204[4];
    s32 unk208;
} UnkStruct;

extern UnkStruct *gSaveProfileImage __asm__("D_8009F92C");
extern u8 gPendingFlag __asm__("D_8009F496");

extern void func_800206D0(void);

/**
 * @brief Arms a profile transfer.
 *
 * @par Confirmed flow
 * func_800206D0() resets the memory-card manager, then the profile context gets
 * unk200 = 6 (state) and unk208 = -0x2B, followed by unk203 = 6 and the pending
 * flag D_8009F496 = 1. func_8007FACC() consumes that flag and is what actually
 * commits the transfer, so this function only prepares it.
 *
 * @par Matching note
 * The $5/$4/$3/$2 pins reproduce the retail register use (6 is built once and
 * reused, the two stores are split across two base registers).
 *
 * @par Callers
 * func_80076A2C, func_8007F8FC, func_8007F93C.
 */
void func_8007FA88(void)
{
    register s32 six __asm__("$5");

    func_800206D0();

    {
        register UnkStruct *p __asm__("$4") = gSaveProfileImage;
        six = 6;
        p->unk200 = six;
        p->unk208 = -0x2B;
    }
    {
        register UnkStruct *q __asm__("$3") = gSaveProfileImage;
        register s32 one __asm__("$2") = 1;
        q->unk203 = six;
        gPendingFlag = one;
    }
}
