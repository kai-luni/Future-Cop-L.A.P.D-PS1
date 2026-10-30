#include "game/types.h"

struct Vec3C2A4 {
    s32 x;
    s32 y;
    s32 z;
};

struct UnkC2A4 {
    u8 unk00[8];
    struct Vec3C2A4 position;
    u8 unk14;
    u8 unk15[3];
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    u8 unk1E[0x14];
    u16 flags;
};

struct OutC2A4 {
    u8 unk00[0x10];
    struct Vec3C2A4 position;
    u8 unk1C[0x2D];
    u8 unk49;
};

extern s32 D_8009F310;
extern struct UnkC2A4 *D_800A7040[][0x100];

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800188D0(s32 arg0, s32 arg1, s32 arg2, s32 *out);
extern s32 func_80066F78(struct Vec3C2A4 *position, s32 arg1, s32 arg2, s32 arg3);
extern void *func_8003FD74(s32 type, s32 arg08, void *source, s32 arg18);

u8 func_8003C2A4(struct UnkC2A4 *data)
{
    struct OutC2A4 *out;
    struct UnkC2A4 *object;
    s32 matrix[9];

    if (data == 0) {
        return 0;
    }
    if ((data->flags & 1) == 0) {
        return 0;
    }
    out = func_80017EBC(0x4C, 2);
    out->unk49 = 4;
    func_800188D0(data->unk18, -data->unk1A, 0, matrix);
    out->position = data->position;
    out->position.x += ((matrix[6] >> 4) * data->unk1C) >> 8;
    out->position.y = func_80066F78(&data->position, 0, 0, 0) +
                      (((matrix[7] >> 4) * data->unk1C) >> 8);
    out->position.z += ((matrix[8] >> 4) * data->unk1C) >> 8;
    func_8003FD74(data->unk14, D_8009F310, 0, (s32)out);
    object = D_800A7040[D_8009F310][data->unk14];
    if (object != 0 && (object->flags & 1) != 0) {
        return 1;
    }
    return 0;
}
