#include "game/types.h"

typedef struct Unk8009F468 {
    u8 pad[0x1C];
    s32 (*unk1C)(s32);
} Unk8009F468;

extern Unk8009F468 *D_8009F468;
extern s32 D_8009F8B8;
extern volatile s32 D_8009FA2C;

s32 func_80082D30(s32 *arg0, s32 arg1)
{
    s32 result = 0;
    s32 (*callback)(s32) = D_8009F468->unk1C;

    if (callback != 0) {
        if (*arg0 != 0) {
            if (callback(D_8009F8B8) & arg1) {
                if (D_8009FA2C - *arg0 >= 0xB5) {
                    result = 1;
                    *arg0 = D_8009FA2C - 0x96;
                }
            }
        }
    }
    return result;
}
