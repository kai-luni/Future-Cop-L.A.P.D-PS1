#include "game/types.h"

extern void func_800451B4(s32 enable);
extern void func_800451F8(s32 volume);
extern void func_80045238(s32 volume);
extern void func_80045294(s32 volume);
extern void func_8001EC04(s32 port, s32 enable);
extern s32 func_80045288(void);
extern s32 func_8002A58C(void);
extern void func_80024630(s32 arg0);
extern void func_8002A16C(s32 arg0);
extern void func_8007BFCC(s32 port, s32 value);

void func_80080984(u8 *arg0, s32 arg1)
{
    func_800451B4(arg0[7]);
    func_800451F8(arg0[4]);
    func_80045238(arg0[5]);
    func_80045294(arg0[6]);
    func_8001EC04(0, arg0[0xA]);
    func_8001EC04(1, arg0[0xB]);
    func_8007BFCC(0, *(s8 *)(arg0 + 8));
    func_8007BFCC(1, *(s8 *)(arg0 + 9));

    if (func_80045288() == 0) {
        if ((func_8002A58C() & 0xFF) == 1) {
            func_80024630(0);
        }
    } else {
        if ((func_8002A58C() & 0xFF) == 0) {
            func_8002A16C(1);
        }
    }
}
