#include "game/types.h"

struct Command { u8 op; u8 arg; };
struct Entity {
    struct Entity *next;
    void (*update)(s32, struct Entity *, void *, s32);
    s32 id;
    struct Command commands[4];
    u8 pad14[2];
    u8 kind;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    s16 unk1A;
    s16 health;
    u8 pad1E[3];
    u8 unk21;
    s8 sound;
    u8 unk23;
    u32 flags;
    u8 pad28[0x6A];
    u8 unk92;
    u8 unk93;
    u8 pad94[0x48];
    struct Entity *childDC;
    u8 padE0[0x78];
    struct Entity *children[4];
};
struct EntityTable { s32 count; s32 unk04; struct Entity *items[1]; };
struct Message { s32 unk0; s16 value; u8 kind; u8 unk7; s32 unk8; };
extern struct EntityTable D_800A8F90;
extern void func_800457B8(s32, s32);
extern void func_8004D958(struct Entity *);
extern void func_8003FD10(s32, struct Entity *, s32);
extern void func_8002A5B4(void *, s32);

void func_8004CECC(void)
{
    s32 i, index, arg;
    register s32 j __asm__("$5");
    register s32 op __asm__("$4");
    struct Entity *entity, *child;
    register struct Entity *cursor __asm__("$6");
    register struct Entity *healthCursor __asm__("$4");
    register struct Entity *command __asm__("$19");
    struct Message message;
    register s32 health __asm__("$2");
    register s32 childHealth __asm__("$6");
    register s32 highByte __asm__("$7");
    s32 mask;
    for (i = 0; i < D_800A8F90.count; i++) {
        entity = D_800A8F90.items[i];
        if (entity->id <= 0) continue;
        op = entity->commands[0].op;
        index = 0;
        if (op == 0) continue;
        command = entity;
        do {
            __asm__("" : "+r"(command));
            arg = command->commands[0].arg;
            switch (op) {
            case 0x39:
                if (entity->flags & 0x400000) {
                    func_800457B8(entity->id, entity->sound);
                    entity->flags &= ~0x400000;
                }
                entity->unk21 = arg;
                break;
            case 0x3A:
                entity->unk1A = arg;
                break;
            case 0x3C:
                entity->flags &= ~0x1000080;
                message.unk0 = 0;
                message.kind = 1;
                message.unk8 = 0;
                if (arg < 2) {
                    mask = ~entity->health >> 31;
                    message.value = (entity->health + 1) & mask;
                } else {
                    message.value = arg;
                }
                if (message.value > 0) entity->update(14, entity, &message, 0);
                break;
            case 0x3D:
                entity->flags |= 0x4000000;
                if (arg == 1) entity->flags |= 0x8000000;
                func_8004D958(entity);
                break;
            case 0x3E:
                entity->unk23 = arg;
                break;
            case 0x3F:
                if (arg) entity->flags |= 0x1000080;
                else entity->flags &= ~0x1000080;
                break;
            case 0x40:
                if (arg) entity->flags |= 0x20000;
                else entity->flags &= ~0x20000;
                break;
            case 0x38:
                health = arg * 100;
                entity->health = health;
                switch (entity->kind) {
                case 20:
                    child = entity->children[0];
                    if (child) child->health = health;
                    break;
                case 28:
                    j = 0;
                    childHealth = health;
                    healthCursor = entity;
                    for (; j < 4; j++) {
                        child = healthCursor->children[0];
                        if (child) child->health = childHealth;
                        healthCursor = (struct Entity *)((u8 *)healthCursor + 4);
                    }
                    break;
                }
                break;
            case 0x41:
                entity->health = arg;
                break;
            case 0x42:
                entity->unk17 = arg;
                break;
            case 0x43:
                entity->unk18 = arg;
                break;
            case 0x44:
                entity->unk92 = 0;
                entity->unk93 = arg;
                if (entity->kind == 1 || entity->kind == 20 || entity->kind == 28) {
                    j = 0;
                    __asm__("" : "+r"(arg));
                    highByte = arg >> 8;
                    cursor = entity;
                    do {
                        child = 0;
                        switch (entity->kind) {
                        case 1: child = entity->childDC; break;
                        case 20: child = entity->children[0]; break;
                        case 28: child = cursor->children[0]; break;
                        }
                        if (child) {
                            child->unk92 = highByte;
                            child->unk93 = arg;
                        }
                        j++;
                        if (entity->kind != 28) break;
                        cursor = (struct Entity *)((u8 *)cursor + 4);
                    } while (j < 4);
                }
                break;
            case 0x64:
            case 0x65:
                func_8003FD10(op, entity, arg);
                break;
            case 0x3B:
                if (arg) entity->flags &= ~4;
                else entity->flags |= 4;
                break;
            case 0x45:
                if (arg) entity->flags &= ~0x20;
                else entity->flags |= 0x20;
                if (entity->kind == 1 || entity->kind == 20 || entity->kind == 28) {
                    j = 0;
                    cursor = entity;
                    do {
                        child = 0;
                        switch (entity->kind) {
                        case 1: child = entity->childDC; break;
                        case 20: child = entity->children[0]; break;
                        case 28: child = cursor->children[0]; break;
                        }
                        if (child) {
                            if (arg) child->flags &= ~0x20;
                            else child->flags |= 0x20;
                        }
                        j++;
                        if (entity->kind != 28) break;
                        cursor = (struct Entity *)((u8 *)cursor + 4);
                    } while (j < 4);
                }
                break;
            default:
                command = (struct Entity *)((u8 *)command + 2);
                index++;
                goto next;
            }
            func_8002A5B4(entity, index);
        next:;
        } while (index < 4 && (op = command->commands[0].op) != 0);
    }
}
