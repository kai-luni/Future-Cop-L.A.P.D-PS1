#include "game/types.h"

struct Unk3F2AC {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[7];
    u8 unk0F;
    u8 unk10;
    u8 unk11[3];
    s16 unk14;
    u8 unk16[2];
    u16 unk18;
    u16 unk1A;
    u8 *unk1C;
    u8 *unk20;
    u8 unk24[0x48];
    void *unk6C;
};

extern s32 func_8003CAE4(struct Unk3F2AC *object, s32 arg, u8 *flag);
extern struct Unk3F2AC *func_8003E984(void *handle, struct Unk3F2AC *object,
                                      s32 flag, s32 arg3, s32 arg4);
extern void func_800498F0(void *object);

s32 func_8003F2AC(struct Unk3F2AC *object, s32 arg1)
{
    u8 flag;
    s32 result;
    struct Unk3F2AC *child;

    flag = 0;
    if (!(object->unk0F & 1)) {
        if ((object->unk0F & 8) &&
            (object->unk07 == -1 || object->unk04 <= 0)) {
            object->unk0F |= 1;
        }
        result = func_8003CAE4(object, arg1, &flag);
        if (flag != 0) {
            child = func_8003E984(object->unk1C, object, flag, 0,
                                  (object->unk10 >> 1) & 1);
            if (child != 0) {
                child->unk18 = object->unk18;
                if (result & 0xFF) {
                    child->unk07 = -1;
                    child->unk04 = -1;
                    child->unk14 = 0;
                    child->unk0F |= 2;
                    child->unk1A &= 0xF7FF;
                }
            }
        }
    }
    if (object->unk0F & 1) {
        if (object->unk07 >= 0 || object->unk04 > 0) {
            object->unk0F = (object->unk0F & ~1) | 8;
            if (object->unk20[4] == 0x5D) {
                if (object->unk6C != 0) {
                    func_800498F0(object->unk6C);
                    object->unk6C = 0;
                }
            }
        }
    }
    return object->unk0F & 1;
}
