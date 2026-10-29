#include "game/types.h"

struct Resource {
    u8 *data;
    u8 unk04[4];
    s32 unk08;
    struct Resource *next;
    struct Resource *prev;
    u8 unk14[4];
    s32 group;
    s32 name;
};

struct Entry511E4 {
    u8 flags00;
    u8 unk01;
    s16 coordinate02;
    s16 coordinate04;
    u16 flags06;
    u8 unk08[4];
};

struct Header511E4 {
    s16 unk00;
    s16 count;
    u8 unk04[4];
};

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

extern s32 D_8009F7B8;
extern struct Header511E4 *D_800A2210[];

extern s32 func_80019AA8(struct Resource *resource);
extern void func_80019A74(struct Resource *resource);
extern void func_80019D00(struct Resource *resource);
extern s32 func_80067684(struct Vec3s32 *position, s32 mode);

void func_800511E4(struct Resource *resource)
{
    struct Vec3s32 position;
    struct Header511E4 *header;
    struct Entry511E4 *entry;
    u8 *data;
    s32 count;
    s32 i;
    s32 type;
    s32 mode;
    s32 value;
    s32 x;
    s32 z;

    if ((u8)func_80019AA8(resource) == 0) {
        resource->unk08 = 0;
        func_80019A74(resource);

        data = resource->data;
        header = (struct Header511E4 *)(data + 0xC);
        D_800A2210[D_8009F7B8] = header;
        count = header->count;
        D_8009F7B8++;

        if (count > 0) {
            entry = (struct Entry511E4 *)(data + 0x14);
            for (i = 0; count > 0; i++) {
                position.x = entry[i].coordinate04 * 256;
                position.z = entry[i].coordinate02 * 256;
                type = entry[i].flags06 & 0xF;

                if (type == 5) {
                    value = (s16)entry[i].flags06 * 16;
                } else {
                    if (type == 0) {
                        mode = -1;
                    } else if (type == 1) {
                        mode = -2;
                    } else {
                        mode = type - 2;
                    }
                    value = func_80067684(&position, mode);
                }

                count--;
                x = position.x;
                entry[i].coordinate04 = ((s32)((u32)value + 0x7FU)) >> 8;
                entry[i].coordinate02 = x >> 8;
                z = position.z;
                entry[i].flags00 &= 0xF0;
                entry[i].flags06 = z >> 8;
            }
        }
    } else {
        func_80019D00(resource);
    }
}
