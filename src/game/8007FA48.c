#include "game/types.h"

extern u8 *D_8009F92C;
extern void func_800181D4(void *block);
extern void func_80020600(void);

void func_8007FA48(void)
{
    func_80020600();
    func_800181D4(*(void **)(D_8009F92C + 0x904));
    func_800181D4(D_8009F92C);
}
