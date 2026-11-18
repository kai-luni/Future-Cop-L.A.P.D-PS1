# 1 "tmp_exp.c"

# 1 "/home/kai-luni/git/future-cop-lapd-ps1/include/game/types.h" 1



typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

typedef char assert_s8_size[sizeof(s8) == 1 ? 1 : -1];
typedef char assert_u16_size[sizeof(u16) == 2 ? 1 : -1];
typedef char assert_u32_size[sizeof(u32) == 4 ? 1 : -1];


# 2 "tmp_exp.c" 2

struct Poly {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
};
extern struct Poly *D_8009F9F0;
extern struct Poly *D_8009FA28;

void func_80082EF0(s16 x, s16 y, s16 w, s16 h, u32 link, s32 r, s32 g, s32 b)
{

    struct Poly *p;
    s32 x0 = x;
    s32 x1 = x0 + w;
    x = 0;

    if (D_8009F9F0 >= D_8009FA28) {
        h = y + h;
        p = D_8009F9F0;
    } else {
        h = y + h;
        p = D_8009F9F0;
        D_8009F9F0 = (struct Poly *)((u8 *)p + 0x34);
    }

    p->len = 5;
    p->code = 0x28;
    p->r = r;
    p->g = g;
    p->b = b;
    p->xy[0] = x0;
    p->xy[1] = y;
    p->xy[2] = x1;
    p->xy[3] = y;
    p->xy[4] = x0;
    p->xy[5] = h;
    p->xy[6] = x1;
    p->xy[7] = h;
    p->addr = link;

}
