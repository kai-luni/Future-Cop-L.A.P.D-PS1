#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct UnkAD08 {
    s16 angle;
    s16 pitch;
    s32 distance;
    u8 data[0xC];
};

struct UnkFA94 {
    u8 pad00[0x28];
    u8 b28;
    u8 pad29[3];
    u8 b2C;
    u8 pad2D[3];
    u8 b30;
    u8 pad31[3];
    u8 b34;
    u8 pad35[3];
    u8 b38;
    u8 pad39[3];
    u8 b3C;
};

struct UnkAEC4 {
    u8 pad0;
    s8 index;
};

struct Arg0 {
    u8 pad00[8];
    struct Vec3s32 position;
    u8 pad14[0x10];
    u16 field24;
};

struct Entity {
    u8 pad00[0x30];
    s32 field30;
    s32 field34;
    s32 field38;
    s32 field3C;
    s32 field40;
    s32 field44;
    s32 field48;
    s32 field4C;
    s32 field50;
    u8 pad54[0x18];
    s32 field6C;
    u8 pad70[0x20];
    u8 field90;
    u8 field91;
    u8 field92;
    u8 pad93[0xD];
    s16 fieldA0;
    u8 padA2[2];
    u8 fieldA4;
    u8 fieldA5;
    u8 fieldA6;
    u8 fieldA7;
    s16 fieldA8;
    s16 fieldAA;
    s8 fieldAC;
    u8 fieldAD;
    s16 fieldAE;
    s32 fieldB0;
    s32 fieldB4;
    s32 fieldB8;
    s32 fieldBC;
    u8 fieldC0;
    s8 fieldC1;
    u8 padC2[0x12];
    s32 fieldD4;
    s32 fieldD8;
    u8 padDC[0x220];
    s8 field2FC;
    s8 field2FD;
    u8 pad2FE[0x12];
    u8 field310;
    u8 pad311[1];
    s8 field312;
    u8 pad313[7];
    s16 field31A;
    s32 field31C;
    s32 field320;
};

struct Arg1 {
    u8 pad00[0x50];
    struct Vec3s32 position;
    u8 pad5C[0x50];
    struct Entity *entity;
    s16 fieldB0;
    s16 fieldB2;
    s16 fieldB4;
    u8 padB6[8];
    s16 fieldBE;
    s16 fieldC0;
};

extern struct UnkAD08 D_8009AD08[];
extern struct UnkFA94 *D_8009FA94;
extern u8 D_8009AEC4[];

extern s32 func_800564A8(s16 pitch);
extern s32 func_80066F78(struct Vec3s32 *position, s32 arg1, s32 arg2, s32 arg3);
extern void func_8005A070(u8 *object);
extern s32 func_80017994(void);
extern void func_80017B60(s32 id);
extern void func_8005612C(struct Vec3s32 *position, s32 heading, s32 angle, s32 pitch,
                          s32 distance, u8 *data, s32 *out, s32 arg7, s32 arg8);
extern void func_80017860(s32 yaw, s32 pitch, s32 roll);
extern void func_800177AC(struct Vec3s32 *position);

void func_800573A4(struct Arg0 *arg0, struct Arg1 *arg1)
{
    struct Entity *entity;
    struct UnkAD08 *offset;
    struct Vec3s32 position;
    s32 mode;
    s32 i;
    s32 id;
    u32 heading;

    offset = D_8009AD08;
    if (offset[0].distance < 0) {
        s32 *p;
        i = 0;
        p = &offset->distance;
        do {
            *p = func_800564A8(*(s16 *)((u8 *)p - 2));
            p += 5;
            i++;
        } while (i < 9);
    }

    heading = arg0->field24;
    entity = arg1->entity;
    arg1->fieldB0 = heading;
    if ((s32)(heading << 16) < 0) {
        arg1->fieldB0 = -heading;
        entity->field2FD = 1;
        entity->fieldAD = 1;
    } else {
        entity->field2FD = 0;
        entity->fieldAD = 0;
    }

    arg1->fieldB2 = 0;
    arg1->fieldB4 = 0;

    switch (entity->fieldC1) {
    case 0:
        entity->field90 = D_8009FA94->b28;
        entity->field91 = D_8009FA94->b2C;
        entity->field92 = D_8009FA94->b30;
        break;
    case 1:
        entity->field90 = D_8009FA94->b34;
        entity->field91 = D_8009FA94->b38;
        entity->field92 = D_8009FA94->b3C;
        break;
    }

    mode = 2;
    entity->field44 = 0x40000;
    entity->fieldA0 = 0;
    entity->fieldC0 = 0;
    entity->fieldBC = 0;
    entity->field48 = mode;
    entity->fieldA4 = 0;
    entity->fieldA8 = 0;
    entity->field31A = 0;
    entity->field6C = 0;
    entity->fieldAE = 0x384;

    arg1->position = arg0->position;
    arg1->position.y = func_80066F78(&arg1->position, 0, 0, 0);

    entity->field34 = 0x800000;
    entity->field4C = 0x1000000;
    entity->fieldA5 = 3;
    entity->field31C = 0x80000;
    entity->field38 = mode;
    entity->field50 = mode;
    entity->field30 = 0;
    entity->field320 = mode;
    entity->field310 = 0;
    entity->field3C = 0x2000000;
    entity->field40 = mode;
    func_8005A070((u8 *)entity);

    arg1->fieldBE = 0;
    arg1->fieldC0 = 0;

    id = func_80017994();
    entity->fieldAC = id;
    func_80017B60((s8)id);

    entity->fieldA6 = 1;
    entity->fieldA7 = 1;
    *(s16 *)((u8 *)entity + 0xAA) = 0;

    offset = &D_8009AD08[(*(struct UnkAEC4 **)D_8009AEC4)->index];

    func_8005612C(&arg1->position, arg1->fieldB0,
                  arg1->fieldB0 + offset->angle, offset->pitch,
                  offset->distance, offset->data, (s32 *)&position,
                  entity->fieldA8, entity->fieldBC);

    func_80017860(arg1->fieldB0 + offset->angle, offset->pitch, 0);
    func_800177AC(&position);

    *(struct Vec3s32 *)&entity->fieldB0 = *(struct Vec3s32 *)offset->data;

    entity->fieldD4 = arg1->position.y;
    entity->fieldD8 = 0;
    entity->field2FC = -1;
    entity->field312 = -1;
}
