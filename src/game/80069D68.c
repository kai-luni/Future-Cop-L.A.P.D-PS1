#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
};

extern u8 D_8009F840;
extern s32 D_800A3320[];
extern s32 *D_8009F9A4;
extern s32 *D_8009F9F8;
extern s32 *D_8009FA94;
extern s32 *D_8009FAD4;
extern s8 D_800A8540[];

extern s32 func_80019668(void);
extern s32 func_80029A60(s32, struct Entity *, s32);
extern void func_800298EC(s32);
extern void func_80029974(s32);
extern s32 func_8004DE80(s32, s32);
extern void func_8004DA84(struct Entity *, s32, s32, s32);
extern void func_8004DC64(s32, s32, s32);
extern void func_8004DD68(s32, s32, s32);
extern void func_8004DDF4(s32, s32, s32);
extern void func_8004F0F0(s32, s32, s32);
extern void func_8006158C(s32, s32, s32);
extern void func_80061600(s32, s32, s32);
extern void func_800841C4(s32, s32, s32);

s32 func_80069D68(u8 *script, s32 arg1, s32 arg2, struct Entity *entity)
{
    u8 *code;
    s32 *stack;
    s32 depth;
    s32 a;
    s32 b;
    s32 c;
    struct Entity *ent;
    s32 saved;
    s32 op;
    s32 rnd;

    code = script;
    saved = arg1;
    ent = entity;

    if (*code == 0) {
        return 1;
    }
    if (D_8009F840 != 0) {
        return 0;
    }

    depth = 1;
    D_800A3320[0] = depth;
    if (arg2 >= 0) {
        D_800A3320[1] = arg2;
        depth = 2;
    }
    stack = &D_800A3320[depth];

    do {
        op = *code++;
        if (op & 0x80) {
            a = op & 0x7F;
            goto push;
        }
        if (op >= 9) {
            a = *--stack;
            depth--;
            if (op >= 0x1D) {
                b = *--stack;
                depth--;
                if (op >= 0x38) {
                    stack--;
                    depth--;
                    c = *stack;
                }
            }
        }
        switch (op) {
        case 1:
            a = *code++;
            a = ~a;
            goto push;
        case 2:
            a = *code++;
            a += 0x80;
            goto push;
        case 3:
            a = *code++;
            a = (a << 8) | *code++;
            goto push;
        case 4:
            a = *code++;
            a = (a << 8) | *code++;
            a = (a << 8) | *code++;
            a = (a << 8) | *code++;
            goto push;
        case 5:
            a = saved;
            goto push;
        case 16:
            a = D_8009FA94[a];
            goto push;
        case 17:
            a = *(s32 *)(D_8009FA94[a + 0x39]);
            goto push;
        case 18:
            a = D_8009F9A4[a] / 5;
            goto push;
        case 19:
            a = D_8009FAD4[a];
            goto push;
        case 29:
            D_8009FA94[a] = b;
            break;
        case 30:
            *(s32 *)(D_8009FA94[a + 0x39]) = b;
            break;
        case 31:
            D_8009F9A4[a] = b * 5;
            break;
        case 32:
            D_8009FAD4[a] = b;
            break;
        case 21:
            D_8009FA94[a]++;
            goto push;
        case 22:
            D_8009FA94[a + 0x39]++;
            goto push;
        case 23:
            D_8009F9A4[a] += 5;
            goto push;
        case 24:
            D_8009FAD4[a]++;
            goto push;
        case 25:
            D_8009FA94[a]--;
            goto push;
        case 26:
            D_8009FA94[a + 0x39]--;
            goto push;
        case 27:
            D_8009F9A4[a] -= 5;
            goto push;
        case 28:
            D_8009FAD4[a]--;
            goto push;
        case 48:
            D_8009FA94[a] += b;
            goto push;
        case 49:
            D_8009FA94[a + 0x39] += b;
            goto push;
        case 50:
            D_8009F9A4[a] += b * 5;
            goto push;
        case 51:
            D_8009FAD4[a] += b;
            goto push;
        case 52:
            D_8009FA94[a] -= b;
            goto push;
        case 53:
            D_8009FA94[a + 0x39] -= b;
            goto push;
        case 54:
            D_8009F9A4[a] -= b * 5;
            goto push;
        case 55:
            D_8009FAD4[a] -= b;
            goto push;
        case 9:
            a = (a == 0);
            goto push;
        case 10:
            a = D_800A8540[a];
            goto push;
        case 11:
            if (a >= 0) {
                a = a % 32;
                a = D_8009F9F8[a];
            } else {
                b = func_80019668();
                op = b >> 31;
                b = (op + b) ^ op;
                a = b % (-a);
            }
            goto push;
        case 15:
            a = func_80029A60(a, ent, 1);
            goto push;
        case 14:
            a = func_80029A60(a, ent, 0);
            goto push;
        case 13:
            func_80029974(a);
            break;
        case 12:
            func_800298EC(a);
            break;
        case 20:
            if (a == 0) {
        case 8:
                code += *code;
            } else {
                code++;
            }
            break;
        case 33:
            a = (b == a);
            goto push;
        case 34:
            a = (b != a);
            goto push;
        case 35:
            a = (a < b);
            goto push;
        case 36:
            a = (a <= b);
            goto push;
        case 37:
            a = (b < a);
            goto push;
        case 38:
            a = (a >= b);
            goto push;
        case 39:
            a = a + b;
            goto push;
        case 40:
            a = b - a;
            goto push;
        case 41:
            a = a * b;
            goto push;
        case 42:
            a = b / a;
            goto push;
        case 43:
            a = b % a;
            goto push;
        case 44:
            a = (b && a);
            goto push;
        case 45:
            a = (b || a);
            goto push;
        case 46:
            if (b == 0) {
                return a;
            }
            break;
        case 47:
            if (b == 0) {
                a = ent->update(0xC, ent, a, 0);
            } else {
                a = func_8004DE80(b, a);
            }
            goto push;
        case 56:
            if (c == 0) {
                func_8004DA84(ent, b, a, 1);
            } else {
                func_8004DC64(c, b, a);
            }
            break;
        case 58:
            func_8004DDF4(c, b, a);
            break;
        case 57:
            func_8004DD68(c, b, a);
            break;
        case 59:
            func_8004F0F0(c, b, a);
            break;
        case 60:
            func_8006158C(c, b, a);
            break;
        case 61:
            func_80061600(c, b, a);
            break;
        case 62:
            func_800841C4(c, b, a);
            break;
        default:
            break;
        }
        continue;
    push:
        *stack = a;
        stack++;
        depth++;
    } while (*code != 0);

    depth--;
    return D_800A3320[depth];
}
