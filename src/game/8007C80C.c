#include "game/types.h"

extern s32 D_8009F8B8;
extern s8 D_8009F904;
extern s8 D_8009F900[];
extern u8 D_8009F8D8[];
extern u8 D_8009F902[];

extern void func_8007BFCC(s32 port, s32 value);
extern void func_8001EC04(s32 port, s32 enable);
extern s32 func_80046C84(s32 id, s32 arg1);

void func_8007C80C(s32 action)
{
    s32 port;

    port = D_8009F8B8;
    if (port == D_8009F904) {
        switch (action) {
        case 3:
            func_8007BFCC(port, D_8009F900[port]);
            func_8001EC04(port, D_8009F8D8[port]);
            break;
        case 4:
        case 5:
            func_80046C84(0x46, 0);
            D_8009F902[port] ^= 1;
            break;
        case 1:
            D_8009F900[port]++;
            if (D_8009F900[port] >= 4) {
                D_8009F900[port] = 0;
            }
            func_80046C84(0x47, 0);
            break;
        case 2:
            D_8009F900[port]--;
            if (D_8009F900[port] < 0) {
                D_8009F900[port] = 3;
            }
            func_80046C84(0x47, 0);
            break;
        }
    }
}
