#include "game/types.h"

extern s8 D_8009F224;
extern u8 D_8009F608;
extern void func_8001C088(s32, u8);

void func_8001BFA8(void)
{
    func_8001C088((1 - D_8009F224) & 0xFF, D_8009F608);
}
