#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect;

struct FontInfo {
    u8 pad00[4];
    s16 f04;
    s16 f06;
    u8 pad08[8];
    u8 data[1];
};

struct FontEntry {
    s16 f00;
    s16 f02;
    s16 f04;
    s16 f06;
    s16 f08;
    union {
        u16 u;
        s16 s;
    } f0A;
    s16 f0C;
    s16 f0E;
    s32 f10;
    struct FontInfo *info;
    s32 f18;
    u8 data[0x100];
};

extern struct FontEntry D_800A19A0[];
extern u16 D_800A1970[];
extern u16 D_800A1940[];
extern u16 D_800A1E10[];
extern u16 D_8009F76A;
extern u16 D_8009F768;

extern void func_80092F38(Rect *rect, void *data);
extern void func_80092C8C(s32 arg0);

void func_80048BF8(s32 arg0)
{
    struct FontInfo *info;
    s32 size;
    s32 i;
    u8 val;
    Rect rect[2];
    u8 *data;

    if (arg0 < 4) {
        info = D_800A19A0[arg0].info;
        if (D_8009F76A + info->f06 < 0x1FF) {
            size = (info->f04 * info->f06) / 2;
            for (i = 0; i < size; i++) {
                val = i[(u8 *)info + 0x10];
                i[(u8 *)info + 0x10] = ((val & 0xF) << 4) | (val >> 4);
            }

            D_800A1970[arg0] = D_8009F76A;
            D_8009F76A += info->f06;
            rect[0].x = 0x380;
            rect[0].y = D_800A1970[arg0] + 0x100;
            data = (u8 *)info + 0x10;
            rect[0].w = info->f04 / 4;
            rect[0].h = info->f06;
            func_80092F38(&rect[0], data);
            func_80092C8C(0);

            D_800A1940[arg0] = (((D_800A1970[arg0] + 0x100) & 0x100) >> 4) | 0xE | (((D_800A1970[arg0] + 0x100) & 0x200) << 2);

            if (arg0 == 0) {
                rect[1].x = 0x380;
                rect[1].y = 0x1FF;
                rect[1].w = 0x10;
                rect[1].h = 1;
                func_80092F38(&rect[1], D_800A1E10);
                func_80092C8C(0);
                D_8009F768 = 0x7FF8;
            }

            for (i = 0; i < size; i++) {
                val = i[(u8 *)info + 0x10];
                i[(u8 *)info + 0x10] = ((val & 0xF) << 4) | (val >> 4);
            }
        }
    }
}
