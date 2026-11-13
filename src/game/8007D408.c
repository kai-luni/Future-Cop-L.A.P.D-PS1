typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef struct { u8 *entries; int unused; u8 count, min, max; u8 pad[9]; } List;
typedef struct { u8 pad0[0x28]; u8 a; u8 pad29[3]; u8 b; u8 pad2D[3]; u8 c; u8 pad31[3]; u8 d; u8 pad35[3]; u8 e; u8 pad39[3]; u8 f; u8 pad3D[0x1F]; int mode; u8 pad60[0x2C]; int x,y,z; } State;
typedef struct { u8 pad[0x19]; u8 max0,max1,max2; u8 tail[8]; } Entry;
extern State *D_8009FA94;
extern List D_8009C0A8[3];
extern u8 D_8009C090[], D_8009C070[];
extern u8 D_800A362B[];
extern s8 D_8009F924[2], D_8009F926[2], D_8009F928[2];
extern s8 D_8009F880;
extern s8 *D_8009F884;
extern Entry D_8009B4E4[];
extern u8 D_8009C0C5[], D_8009C0D9[];
extern int D_8009F8E0, D_8009F8E4;
typedef struct { u8 pad[0x16]; u16 a; u8 pad2[0x6E]; u16 b; } Tail;
extern Tail D_8009B7D0;
extern u8 func_8007D39C(List *, u8);
#define CLAMP(x,lo,hi) ((x)<(lo)?(lo):(u8)((x)>(hi)?(hi):(x)))
void func_8007D408(void)
{
    if (D_8009FA94->mode == 1) {
        D_8009C0A8[2].entries = D_8009C090;
        D_8009C0A8[2].count = 4;
        D_800A362B[0] = 7;
        D_8009C0A8[1].count = 4;
    } else {
        D_8009C0A8[2].entries = D_8009C070;
        D_8009C0A8[2].count = 5;
        D_800A362B[0] = 2;
        D_8009C0A8[1].count = 5;
    }
    D_8009F924[0] = func_8007D39C(&D_8009C0A8[0], D_8009FA94->a);
    D_8009F924[1] = func_8007D39C(&D_8009C0A8[0], D_8009FA94->d);
    D_8009F926[0] = func_8007D39C(&D_8009C0A8[1], D_8009FA94->b);
    D_8009F926[1] = func_8007D39C(&D_8009C0A8[1], D_8009FA94->e);
    D_8009F928[0] = func_8007D39C(&D_8009C0A8[2], D_8009FA94->c);
    D_8009F928[1] = func_8007D39C(&D_8009C0A8[2], D_8009FA94->f);
    if (D_8009FA94->x) D_8009C0A8[0].min = 0;
    else D_8009C0A8[0].min = 1;
    D_8009C0A8[0].max = D_8009B4E4[D_8009F884[D_8009F880]].max0;
    D_8009F924[0] = CLAMP(D_8009F924[0], D_8009C0A8[0].min, D_8009C0A8[0].max);
    D_8009F924[1] = CLAMP(D_8009F924[1], D_8009C0A8[0].min, D_8009C0A8[0].max);
    if (D_8009FA94->y) D_8009C0C5[0] = 0;
    else D_8009C0C5[0] = 1;
    D_8009C0A8[1].max = D_8009B4E4[D_8009F884[D_8009F880]].max1;
    D_8009F926[0] = CLAMP(D_8009F926[0], D_8009C0A8[1].min, D_8009C0A8[1].max);
    D_8009F926[1] = CLAMP(D_8009F926[1], D_8009C0A8[1].min, D_8009C0A8[1].max);
    if (D_8009FA94->z) D_8009C0D9[0] = 0;
    else D_8009C0D9[0] = 1;
    D_8009C0A8[2].max = D_8009B4E4[D_8009F884[D_8009F880]].max2;
    D_8009F928[0] = CLAMP(D_8009F928[0], D_8009C0A8[2].min, D_8009C0A8[2].max);
    D_8009F928[1] = CLAMP(D_8009F928[1], D_8009C0A8[2].min, D_8009C0A8[2].max);
    D_8009F8E0 = 0;
    D_8009F8E4 = 0;
    D_8009B7D0.a = 100;
    D_8009B7D0.b = 100;
}
