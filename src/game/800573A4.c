// Categories: Gameplay > Object Spawning > Player Setup | Graphics > Camera > Position Update
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

extern struct UnkAD08 gViewPresetTable[] __asm__("D_8009AD08");
extern struct UnkFA94 *gGameSession __asm__("D_8009FA94");
extern u8 gPresetEntryTable[] __asm__("D_8009AEC4");

extern s32 func_800564A8(s16 pitch);
extern s32 func_80066F78(struct Vec3s32 *position, s32 arg1, s32 arg2, s32 arg3);
extern void func_8005A070(u8 *object);
extern s32 func_80017994(void);
extern void func_80017B60(s32 id);
extern void func_8005612C(struct Vec3s32 *position, s32 heading, s32 angle, s32 pitch,
                          s32 distance, u8 *data, s32 *out, s32 arg7, s32 arg8);
extern void func_80017860(s32 yaw, s32 pitch, s32 roll);
extern void func_800177AC(struct Vec3s32 *position);

/**
 * @brief Resets an actor's mobility state from its spawn record and places the
 *        third-person view that belongs to it.
 *
 * @param arg0 Spawn record: a three-word position at +0x08 and the heading at
 *        +0x24 (u16, engine angle units).
 * @param arg1 The object: its world position at +0x50, the pointer to the
 *        0x328 state block at +0xAC, the heading written at +0xB0, +0xB2/+0xB4
 *        and +0xBE/+0xC0 cleared.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. The nine-entry table D_8009AD08 (s16 angle, s16 pitch, s32 distance, 12
 *    data bytes) is resolved lazily: when the first entry's distance is
 *    negative, all nine distances are recomputed with
 *    func_800564A8(entry->pitch).
 * 2. heading = arg0 +0x24 is stored at arg1 +0xB0; when its signed 16-bit value
 *    is negative the stored heading is negated and state +0xAD and +0x2FD are
 *    both set to 1, otherwise both are 0.
 * 3. State +0xC1 selects one of the two colour triples of D_8009FA94
 *    (+0x28/+0x2C/+0x30 for 0, +0x34/+0x38/+0x3C for 1) into state +0x90,
 *    +0x91 and +0x92.
 * 4. The state block is then overwritten: +0x30 = 0, +0x34 = 0x800000, +0x38 =
 *    2, +0x3C = 0x2000000, +0x40 = 2, +0x44 = 0x40000, +0x48 = 2, +0x4C =
 *    0x1000000, +0x50 = 2, +0x6C = 0, +0xA0 = 0, +0xA4 = 0, +0xA5 = 3, +0xA6
 *    and +0xA7 = 1, +0xA8 = 0, +0xAA = 0, +0xAE = 0x384, +0xBC = 0, +0xC0 = 0,
 *    +0x2FC = -1, +0x310 = 0, +0x312 = -1, +0x31A = 0, +0x31C = 0x80000,
 *    +0x320 = 2, and func_8005A070 copies 20 bytes from D_8009AFA0 into
 *    +0x54.
 * 5. The object position is copied from the record and its y replaced by the
 *    ground height (func_80066F78(&arg1 +0x50, 0, 0, 0)), which is also stored
 *    at state +0xD4 while +0xD8 is cleared.
 * 6. A new id comes from func_80017994 (a node appended to the list behind
 *    D_8009F588 with the next running id) and is stored in the byte at state
 *    +0xAC; func_80017B60(id) selects the map/camera record used below.
 * 7. The offset entry is D_8009AD08 indexed by the byte at +0x01 of the record
 *    pointed to by D_8009AEC4. func_8005612C(&arg1 +0x50, heading, heading +
 *    entry->angle, entry->pitch, entry->distance, entry->data, &position,
 *    state +0xA8, state +0xBC) computes the view point, func_80017860(heading +
 *    entry->angle, entry->pitch, 0) sets the camera rotation and
 *    func_800177AC(&position) its position.
 * 8. The entry's 12 data bytes are copied to state +0xB0..+0xBB.
 *
 * @par Likely role
 * The spawn/respawn reset of a player-controlled actor: it clears everything the
 * drive model of func_80055988 relies on, gives the actor its own map record and
 * parks the chase camera behind it using a per-view offset preset. The negative
 * heading case Likely marks a spawn that faces backwards.
 *
 * @par Unknown
 * The meaning of the constants (0x800000, 0x1000000, 0x2000000, 0x40000,
 * 0x80000, 0x384, the 3 at +0xA5) and of +0xAD, which func_80055988 treats as
 * the current form; also what distinguishes the two colour triples and what the
 * 12 data bytes copied into +0xB0 describe.
 *
 * @par Callers
 * func_800579D4, right after it allocated the actor's 0x328 state block.
 */
void func_800573A4(struct Arg0 *arg0, struct Arg1 *arg1)
{
    struct Entity *entity;
    struct UnkAD08 *offset;
    struct Vec3s32 position;
    s32 mode;
    s32 i;
    s32 id;
    u32 heading;

    offset = gViewPresetTable;
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
        entity->field90 = gGameSession->b28;
        entity->field91 = gGameSession->b2C;
        entity->field92 = gGameSession->b30;
        break;
    case 1:
        entity->field90 = gGameSession->b34;
        entity->field91 = gGameSession->b38;
        entity->field92 = gGameSession->b3C;
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

    offset = &gViewPresetTable[(*(struct UnkAEC4 **)gPresetEntryTable)->index];

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
