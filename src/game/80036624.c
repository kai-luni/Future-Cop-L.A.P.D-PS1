#include "game/types.h"

struct Entity {
    u8 pad00[8];
    struct Entity *link;
    u16 flags;
    u8 pad0e[6];
    u16 id;
    u8 type;
    u8 pad17;
    u8 kind;
    u8 pad19;
    s16 val1a;
    s16 val1c;
    u8 pad1e[0x32];
    s32 pos[3];
    u8 pad5c[0x44];
    s16 x, y, z, radius;
    u8 pada8[0xc];
    struct Entity *owner;
    u8 padb8[0x10];
    struct Entity *other;
    u8 padcc[4];
    s16 strength;
    u8 padd2[0xa];
    u8 mode;
    u8 paddd[3];
    s32 state;
    u32 flags2;
    u8 pade8[4];
    struct Entity *target;
};
extern void func_80035904(struct Entity *, struct Entity *, s32);
extern void func_80035B7C(struct Entity *, struct Entity *);
extern s32 func_8002AD58(s32, struct Entity *, struct Entity *, s32);
extern s32 func_80019FCC(s32 *);
extern s32 func_80095900(s32);
extern void func_800410BC(void *, s32, s32, struct Entity *);

s32 func_80036624(s32 msg, struct Entity *self, struct Entity *data, s32 extra)
{
    s32 vec[3];
    s32 v, diff;
    s16 amount;
    struct Entity *other;
    struct Entity *owner;
    register struct Entity *incoming __asm__("$5");
    switch (msg) {
    case 1:
        return 0xfc;
    case 2:
        func_80035904(self, data, extra);
        return 0;
    case 6:
        func_80035B7C(self, data);
        return 0;
    case 10:
        other = self->other;
        incoming = data;
        if (other) {
            switch (other->type) {
            case 0x12:
                owner = other->owner;
                goto check_owner;
            case 0x15:
                owner = other->target;
            check_owner:
                if (owner == data) return 0;
            }
        }
        if (incoming->type == 2 && incoming->kind == self->kind) return 0;
        if (self->other && (self->other->id & 0x3fff) == (incoming->id & 0x3fff)) return 0;
        return self->other != incoming;
    case 11:
        if (data->type == 14 && (data->pad5c[0x44] & 8)) return 0;
        if (self->mode == 0x15) {
            v = data->val1c;
            diff = v - data->val1a;
            self->state = 4;
            v -= (diff >> 31) & diff;
            diff = v - 2;
            v -= (diff >> 31) & diff;
            self->val1c -= v;
        } else {
            self->val1c = -1;
        }
        return 0;
    case 9:
        if (!data->link) return 0;
        if ((data->flags & 1) && data->link->kind == self->kind) return 0;
        func_8002AD58(msg, self, data, extra);
        return 0;
    case 4:
        if (self->strength > 0) {
            if (self->flags2 & 1) {
                vec[0] = (self->x << 8) - self->pos[0];
                vec[1] = (self->y << 8) - self->pos[1];
                vec[2] = (self->z << 8) - self->pos[2];
                amount = self->val1a - (((func_80095900(func_80019FCC(vec)) << 2) >> 8) * self->val1a / self->radius);
            } else {
                amount = self->val1a;
            }
            if (amount > 0) func_800410BC(self->pos, amount, self->strength << 8, self->other);
        }
        break;
    case 5:
        return 0;
    }
    return func_8002AD58(msg, self, data, extra);
}
