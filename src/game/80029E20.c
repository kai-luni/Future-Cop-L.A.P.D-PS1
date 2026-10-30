#include "game/types.h"

struct Unk29E20 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern void func_80019A74(struct Unk29E20 *resource);
extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 arg2, s32 arg3);
extern u8 *D_8009F698;

void func_80029E20(struct Unk29E20 *resource)
{
    func_80019A74(resource);
    D_8009F698 = func_80019C38(resource->unk00, resource->unk20, 0x74544F53,
                               resource->unk1C);
}
