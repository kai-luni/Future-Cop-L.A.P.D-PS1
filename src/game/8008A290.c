#include "game/types.h"

typedef void (*Handler)(s32 port);

extern s32 D_8009C77C, D_8009C778, D_8009C760, D_8009C75C;
extern Handler D_8009C72C;
extern s32 D_800A3B70[], D_800A3B74[];

s32 func_8008A290(s32 arg)
{
    s32 combined = (D_8009C77C << 1) | (D_8009C778 == 0);
    if (combined != arg) {
        D_8009C760 = 0;
        if (arg & 1) {
            register s32 *p __asm__("$18");
            s32 t;
            __asm__("lui %0,%%hi(D_800A3B70)\n\taddiu %0,%0,%%lo(D_800A3B70)" : "=r"(p));
            t = p[0];
            D_8009C778 = 0;
            if (t >= 150) D_8009C72C(D_8009C75C);
            p[0] = 0;
        } else D_8009C778 = 1;
        if (arg & 2) {
            register s32 one __asm__("$3") = 1;
            register s32 *q __asm__("$16");
            s32 t;
            __asm__ volatile("" : : "r"(one));
            q = D_800A3B74;
            __asm__ volatile("" : "=r"(q) : "0"(q));
            t = q[0];
            D_8009C77C = one;
            if (t >= 150) D_8009C72C(D_8009C75C + 0xF0);
            q[0] = 0;
        } else D_8009C77C = 0;
        D_8009C760 = 1;
    }
    return combined;
}
