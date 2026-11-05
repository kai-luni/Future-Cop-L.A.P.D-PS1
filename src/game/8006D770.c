#include "game/types.h"

typedef struct Obj Obj;
typedef struct Sub { u8 pad[0x2e]; u16 field2e; } Sub;
struct Obj {
    Obj *next;
    u8 pad04[0xb];
    u8 flags;
    u8 pad10[2];
    s16 field12;
    u8 pad14[0xc];
    Sub *sub;
    u8 pad24[0x10];
    u8 matrix[0x34];
    s16 field68, field6a, field6c, field6e, field70;
    s16 field72;
    s16 field74, field76, field78, field7a, field7c, field7e;
    s16 field80, field82, field84, field86;
    u8 pad88;
    u8 c89, c8a, c8b, pad8c, c8d, c8e, c8f;
    s16 field90, field92, field94, field96;
    u16 field98;
    s16 field9a;
};
extern s32 D_8009FA50;
extern void func_8006D520(Obj *);
extern void func_800188D0(s32, s32, s32, void *);

void func_8006D770(Obj *node, s32 dt)
{
    Obj *p;
    s32 amount;
    u8 mask;
    s32 timer;
    s16 rem;
    s32 col;
    s32 x, y;
    s32 frame;
    for (; node; node = node->next) {
        if (node->flags & 8) continue;
        p = node;
        if (!p) continue;
        if (!(p->field98 & 1) && p->field86 != 0) {
            amount = (p->field86 * dt) >> 8;
            mask = 0;
            if (p->c89 < amount) { mask |= 1; p->c89 = 0; } else p->c89 -= amount;
            if (p->c8a < amount) { mask |= 2; p->c8a = 0; } else p->c8a -= amount;
            if (p->c8b < amount) { mask |= 4; p->c8b = 0; } else p->c8b -= amount;
            if (p->c8d < amount) { mask |= 8; p->c8d = 0; } else p->c8d -= amount;
            if (p->c8e < amount) { mask |= 16; p->c8e = 0; } else p->c8e -= amount;
            if (p->c8f < amount) { mask |= 32; p->c8f = 0; } else p->c8f -= amount;
            if (mask == 63) {
                if (p->sub->field2e) func_8006D520(p);
                p->flags |= 1;
            }
        }
        timer = (u16)p->field76 - dt;
        p->field76 = timer;
        if ((s16)timer <= 0) {
            p->field76 = -p->field76;
            *(volatile s16 *)&p->field72 += 1 + p->field76 / 20;
            frame = p->field72;
            rem = frame % p->field74;
            *(volatile s16 *)&p->field72 = rem;
            col = rem % p->field84;
            p->field76 = 20 - p->field76 % 20;
            x = p->field72 / p->field84;
            p->field7c = p->field78 + col * p->field80;
            p->field7e = p->field7a + x * p->field82;
        }
        p->field12 += (p->field68 * dt) >> 8;
        if (*(s32 *)&p->field94) {
            p->field90 = (p->field90 + (s16)((dt * p->field94) >> 8)) % 4096;
            p->field92 = (p->field92 + (s16)((dt * p->field96) >> 8)) % 4096;
            func_800188D0(p->field90, 1024 - p->field92, 0, p->matrix);
        }
        if (p->field6a > 0) {
            p->field68 -= (p->field6a * dt) >> 7;
            if (p->field68 < 4096) { p->field68 = 4096; p->field6a = 0; }
        }
        if (p->sub->field2e && (D_8009FA50 & 3) == 3) func_8006D520(p);
        if (p->field12 >= p->field6e) {
            if (p->sub->field2e && p->field9a != p->field12) func_8006D520(p);
            p->flags |= 1;
        }
    }
}
