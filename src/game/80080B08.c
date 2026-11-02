#include "game/types.h"

extern u8 *D_8009F92C;
extern s32 D_8009F464;

extern void func_8007FB04(void);

void func_80080B08(void)
{
    u8 s;

    func_8007FB04();
    s = D_8009F92C[0x200];
    switch (s) {
    case 1:
    case 6:
        break;
    case 2:
        if (D_8009F92C[0x201] != 1) {
            D_8009F464 = 3;
        }
        break;
    default:
        D_8009F464 = 3;
        break;
    }
}
