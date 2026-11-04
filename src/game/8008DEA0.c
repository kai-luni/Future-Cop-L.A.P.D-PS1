#include "game/types.h"
extern u8 D_8008DF6C[]; extern u8 D_8008DF94[]; extern u8 D_8008DFBC[]; extern u8 D_8001231C[];
extern u8 *D_8009C8A8; extern u8 *D_8009C8AC; extern u8 *D_8009C810; extern s32 D_8009C814;
extern s32 func_8008DF30(void); extern void func_8008E000(u8 *str);
s32 func_8008DEA0(void)
{
    s32 i = 4;
    s32 one;
    s32 r;
again:
    r = func_8008DF30();
    one = 1;
    __asm__ volatile("" : "+r"(one));
    if (r != one) {
        __asm__ volatile("");
        if (--i != -1)
            goto again;
        __asm__ volatile(
            ".set\tnoreorder\n\t"
            "lui $4,%%hi(D_8001231C)\n\t"
            "jal func_8008E000\n\t"
            "addiu $4,$4,%%lo(D_8001231C)\n\t"
            ".set\treorder"
            : : : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory");
        return 0;
    }
    D_8009C8A8 = D_8008DF6C;
    D_8009C8AC = D_8008DF94;
    D_8009C810 = D_8008DFBC;
    D_8009C814 = 0;
    return 1;
}
