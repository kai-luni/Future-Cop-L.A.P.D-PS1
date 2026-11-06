#include "game/types.h"

typedef struct {
    u8 pad00[0x34];
    u16 a34;
    s16 a36;
    u8 colors[9];
    u8 a41;
    s16 a42, a44, a46, a48, a4a, a4c, a4e, a50, a52, a54;
    u16 a56;
} Src;

typedef struct {
    u8 pad00[8];
    s16 a08, a0a;
    u8 pad0c[6];
    u16 a12;
    u8 pad14[0x10];
    s32 a24;
    u8 pad28[0x3c];
    u8 pool[12];
    s16 a70;
    u8 a72;
    u8 colors[3][3];
    s16 slopes[3][3];
    u16 a8e;
    s16 a90, a92, a94, a96, a98, a9a, a9c, a9e;
} Dest;

typedef struct {
    u8 pad[0x18];
    s32 x, y, z;
} Work;

extern u8 D_80075698[];
extern void *func_80017EBC(s32, s32);
extern void func_800181D4(void *);
extern u8 func_8003BE2C(void *, s32, s32);
extern void func_8003C14C(void *, void *);
extern u32 func_80019668(void);
extern void func_800188D0(s32, s32, s32, Work *);

void *func_800762B0(Src *arg)
{
    register Src *src __asm__("$17");
    Dest *dest;
    s16 time[3];
    Work work;
    register s32 x __asm__("$18");
    register s32 angle __asm__("$4");
    register s32 second __asm__("$2");
    register s32 y __asm__("$5");
    s32 i;
    register u32 flags __asm__("$5");
    register s32 a50 __asm__("$4");
    register s32 a52 __asm__("$3");
    register s32 a4e __asm__("$2");

    src = arg;
    dest = func_80017EBC(0xA0, 1);
    if (!dest) return 0;
    if (func_8003BE2C(dest->pool, 0x30, 0x28) != 1) {
        func_800181D4(dest);
        return 0;
    }
    func_8003C14C(dest, src);
    dest->a24 = (s32)D_80075698;
    dest->a72 = src->a41;
    dest->a12 = src->a34;
    dest->a90 = src->a48;
    dest->a94 = src->a50;
    dest->a96 = src->a52;
    dest->a98 = src->a54;
    angle = src->a4a;
    x = angle;
    if (angle < 0) x = func_80019668() % -src->a4a;
    second = src->a4c;
    if (second < 0) {
        y = func_80019668() % -src->a4c;
        angle = x;
    } else {
        y = second;
        angle = x;
    }
    func_800188D0(angle, -y, 0, &work);
    dest->a9a = ((work.x >> 4) * src->a4e) >> 12;
    dest->a9c = ((work.y >> 4) * src->a4e) >> 12;
    dest->a9e = ((work.z >> 4) * src->a4e) >> 12;
    time[0] = src->a42;
    time[1] = src->a44;
    time[2] = src->a46;
    dest->colors[0][0] = src->colors[0];
    dest->colors[1][0] = src->colors[1];
    dest->colors[2][0] = src->colors[2];
    dest->colors[0][1] = src->colors[3];
    dest->colors[1][1] = src->colors[4];
    dest->colors[2][1] = src->colors[5];
    dest->colors[0][2] = src->colors[6];
    dest->colors[1][2] = src->colors[7];
    dest->colors[2][2] = src->colors[8];
    for (i = 0; i < 3; i++) {
        if (time[i] < 16) {
            dest->slopes[0][i] = 0;
            dest->slopes[1][i] = 0;
            dest->slopes[2][i] = 0;
        } else if (i == 2) {
            dest->slopes[0][2] = (-dest->colors[0][2] << 12) / time[2];
            dest->slopes[1][2] = (-dest->colors[1][2] << 12) / time[2];
            dest->slopes[2][2] = (-dest->colors[2][2] << 12) / time[2];
        } else {
            dest->slopes[0][i] = ((dest->colors[0][i+1] - dest->colors[0][i]) << 12) / time[i];
            dest->slopes[1][i] = ((dest->colors[1][i+1] - dest->colors[1][i]) << 12) / time[i];
            dest->slopes[2][i] = ((dest->colors[2][i+1] - dest->colors[2][i]) << 12) / time[i];
        }
    }
    dest->a70 = time[0] + time[1] + time[2];
    if (dest->a70 != 0) dest->a70 = (src->a36 << 12) / dest->a70;
    flags = src->a56;
    a50 = src->a50;
    a4e = src->a4e;
    a52 = src->a52;
    if (a4e != 0) {
        a4e = a52 | 1;
        a4e = a50 | a4e;
        if (a4e) goto flag_set;
        dest->a8e = flags;
        goto flag_done;
    }
    a4e = a50 | a52;
    if (a4e) goto flag_set;
    goto flag_store;
flag_set:
    flags |= 0x8000;
flag_store:
    dest->a8e = flags;
flag_done:
    if (src->a48 < ((src->a4e * (time[0] + time[1] + time[2])) >> 12)) {
        a4e = flags | 0x4000;
        dest->a8e = a4e;
    }
    dest->a0a = 10;
    dest->a08 = 0;
    return dest;
}
