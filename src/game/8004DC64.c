#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x08];
    u16 unk14;
};

extern struct Entity *D_8009FA40;

extern void func_80031BEC(s32 id, s32 kind, s32 mode);
extern void func_8003FD6C();
extern void func_8004953C(s32 index, s32 kind, s32 value);
extern u8 func_8004DA84(struct Entity *entity, s32 kind, s32 value, s32 retry);

void func_8004DC64(s32 arg0, s32 kind, s32 value)
{
    struct Entity *entity;

    switch (kind) {
    case 0x7C:
        func_80031BEC(arg0, 0x7C, value);
        return;
    case 0x7E:
        func_8004953C(arg0, 0x7E, value);
        return;
    case 0x69:
        func_8003FD6C(arg0, 0x69, value);
        return;
    }

    for (entity = D_8009FA40; entity != 0; entity = entity->next) {
        if (entity->unk14 == arg0 && entity->id != 0) {
            func_8004DA84(entity, kind, value, 1);
        }
    }
}
