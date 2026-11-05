#include "game/types.h"

struct Unk3D240 {
    struct Unk3D240 *next;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    struct Unk3D240 *unk34;
    u8 unk38;
    u8 flags39;
    s16 unk3A;
    u16 unk3C;
    s16 unk3E;
    u8 unk40[4];
    s32 unk44;
    s32 unk48;
};

struct Unk3D240Transform {
    s32 matrix[9];
    s32 pos[3];
};

extern struct Unk3D240 *D_8009F320;

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800181D4(void *block);
extern void func_8003CF20(struct Unk3D240 *object);
extern void func_8003D0F0(void *source, s32 a1, s32 a2, void *destination);

void func_8003D240(void)
{
    struct Unk3D240 *object;
    struct Unk3D240 *prev;
    struct Unk3D240 *next;
    struct Unk3D240Transform local;
    s32 first;

    prev = 0;
    object = D_8009F320;
    while (object != 0) {
        next = object->next;
        if (object->unk38 == 0 && object->unk04 != 0 &&
            *(s32 *)((u8 *)object->unk04 + 8) != object->unk08) {
            if (!(object->flags39 & 4)) {
                object->unk34 = func_80017EBC(0x4C, 1);
                if (object->unk34 != 0) {
                    *(struct Unk3D240 *)object->unk34 =
                        *(struct Unk3D240 *)((u8 *)object->unk04 + 0x28);
                    object->flags39 |= 2;
                    goto tail;
                }
            }
            {
                register struct Unk3D240 *x __asm__("$2");
                register struct Unk3D240 *sub __asm__("$3");
                register s32 m1 __asm__("$2");
                register s32 first __asm__("$3");
                s32 m8;
                s32 pos2;
                x = object->unk04;
                sub = x;
                m1 = *(s32 *)((u8 *)x + 0x30);
                first = *(s32 *)((u8 *)sub + 0x2C);
                local.matrix[1] = m1;
            local.matrix[2] = *(s32 *)((u8 *)object->unk04 + 0x34);
            local.matrix[3] = *(s32 *)((u8 *)object->unk04 + 0x38);
            local.matrix[4] = *(s32 *)((u8 *)object->unk04 + 0x3C);
            local.matrix[5] = *(s32 *)((u8 *)object->unk04 + 0x40);
            local.matrix[6] = *(s32 *)((u8 *)object->unk04 + 0x44);
            local.matrix[7] = *(s32 *)((u8 *)object->unk04 + 0x48);
            local.matrix[8] = *(s32 *)((u8 *)object->unk04 + 0x4C);
            local.pos[0] = *(s32 *)((u8 *)object->unk04 + 0x50);
            local.pos[1] = *(s32 *)((u8 *)object->unk04 + 0x54);
            pos2 = *(s32 *)((u8 *)object->unk04 + 0x58);
            local.matrix[0] = first;
            local.pos[2] = pos2;
            func_8003D0F0((u8 *)object->unk04 + 0x28, -1, 0, (u8 *)object + 0x28);
            object->unk04 = local.matrix[0];
            object->unk08 = local.matrix[1];
            object->unk0C = local.matrix[2];
            object->unk10 = local.matrix[3];
            object->unk14 = local.matrix[4];
            object->unk18 = local.matrix[5];
            object->unk1C = local.matrix[6];
            object->unk20 = local.matrix[7];
            m8 = local.matrix[8];
            object->unk28 += local.pos[0];
            object->unk24 = m8;
            object->unk2C += local.pos[1];
            object->unk30 += local.pos[2];
                object->unk34 = 0;
            }
        tail:
            object->unk38 = 1;
            object->flags39 |= 1;
        }
        if (object->unk3E == 0) {
            object->flags39 |= 8;
        }
        if (object->unk34 != 0 &&
            (*(u32 *)((u8 *)object + 0x38) & 0xA00) == 0xA00) {
            object->unk04 = object->unk34->unk04;
            object->unk08 = object->unk34->unk08;
            object->unk0C = object->unk34->unk0C;
            object->unk10 = object->unk34->unk10;
            object->unk14 = object->unk34->unk14;
            object->unk18 = object->unk34->unk18;
            object->unk1C = object->unk34->unk1C;
            object->unk20 = object->unk34->unk20;
            object->unk24 = object->unk34->unk24;
            object->unk28 = object->unk34->unk28;
            object->unk2C = object->unk34->unk2C;
            object->unk30 = object->unk34->unk30;
            func_8003D0F0(object->unk34, -1, 0, (u8 *)object + 0x28);
            object->unk28 += object->unk34->unk28;
            object->unk2C += object->unk34->unk2C;
            object->unk30 += object->unk34->unk30;
            func_800181D4(object->unk34);
            object->unk34 = 0;
            object->unk38 = 1;
            object->flags39 = (object->flags39 & 0xF5) | 1;
        }
        if (object->unk3A == 0) {
            if (prev != 0) {
                prev->next = object->next;
            } else {
                D_8009F320 = object->next;
            }
            object->next = 0;
            func_8003CF20(object);
        } else {
            prev = object;
        }
        object = next;
    }
}
