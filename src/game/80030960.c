#include "game/types.h"

typedef struct Object {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x8C];
    u32 unkB4;
    u8 padB8[2];
    s16 unkBA;
    u8 unkBC;
    u8 unkBD;
    u8 padBE[0x16];
    u8 unkD4[4];
} Object;

extern void *jtbl_800111DC[];
extern u32 D_8009FAA4;
extern void func_800302E8(Object *, void *, s32);
extern u8 func_8002F8B8(u8, s16, void *, u8);
extern void func_80030498(Object *, void *);
extern s32 func_8002AD58(s32, Object *, void *);
extern u8 func_80017508(void *, u32 *, s32);
extern s32 func_800306A0(Object *, void *);

s32 func_80030960(s32 message, Object *object, void *data, s32 arg3)
{
    register Object *self __asm__("$16");
    register void *other __asm__("$17");
    u32 index;
    u32 flags;
    static void *const labels[] __attribute__((used)) = {
        &&case1, &&case2, &&case3, &&fallback, &&fallback,
        &&case6, &&fallback, &&fallback, &&fallback, &&case10
    };
    self = object;
    other = data;
    index = (u32)message - 1;
    if (index >= 10) goto fallback;
    goto *jtbl_800111DC[index];
case1:
    return 0x134;
case2:
    func_800302E8(self, other, arg3);
    return 0;
case10:
    if (self->unkBD == 2 || self->unkBD == 0) {
        if (func_8002F8B8(self->unkBC, self->unkBA, other, self->unkBD == 2)) {
            func_80030498(self, other);
        }
    }
    return 1;
case3:
    if (self->unkBD != 2 && self->unkBD != 5) goto fallback;
    if (self->flags & 0x20) return 0;
    func_8002AD58(message, self, other);
    if ((self->unkB4 & 0xF0) && (D_8009FAA4 & 0x80)) {
        flags = 0x2000000;
    } else {
        flags = 0;
    }
    return func_80017508(self->unkD4, &flags, 1);
case6:
    return func_800306A0(self, other);
fallback:
    return func_8002AD58(message, self, other);
}
