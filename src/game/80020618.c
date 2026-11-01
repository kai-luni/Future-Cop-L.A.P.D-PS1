#include "game/types.h"

extern u8 *D_8009AB08;

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern int func_80020610(void);

void *func_80020618(s32 arg0, void *arg1)
{
    u32 i;
    u8 *result;

    if (D_8009AB08 == 0) {
        if ((u32)arg0 < 2) {
            D_8009AB08 = arg1;
            func_8001E4C0(D_8009AB08, 0, func_80020610());

            for (i = 0; i < 0x10; i++) {
                ((u32 *)(D_8009AB08 + 0x44))[i] =
                    (u32)(D_8009AB08 + 0xC4 + i * 0x80);
                ((u32 *)(D_8009AB08 + 0x84))[i] =
                    (u32)(D_8009AB08 + 0x104 + i * 0x80);
            }

            result = D_8009AB08;
            *(u32 *)result = (u32)arg0 << 4;
            return result;
        }
    }

    return 0;
}
