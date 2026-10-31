#include "game/types.h"

typedef struct {
    u8 pad00[8];
    void (*callback)(s32 arg0, void *arg1, s32 arg2, void *arg3);
    u8 pad0C[0x14];
    s32 unk20;
} DispatchDDE8;

extern u8 D_80012A94[];
extern DispatchDDE8 *D_8009DDE8;
extern void func_80092CF4(void *arg0, void *arg1);

void func_80092F38(void *rect, void *data)
{
    func_80092CF4(D_80012A94, rect);
    D_8009DDE8->callback(D_8009DDE8->unk20, rect, 8, data);
}
