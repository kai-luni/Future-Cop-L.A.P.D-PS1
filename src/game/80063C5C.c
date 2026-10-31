#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect63C5C;

typedef struct {
    u8 unk00[0x28];
    u8 *unk28;
    u8 *unk2C;
} Res63C5C;

typedef struct {
    void *unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
    u8 unk24[4];
    u8 *unk28;
    u8 *unk2C;
} Arg63C5C;

extern u32 D_800A8E80[];
extern Res63C5C *func_8001996C(void *arg0, s32 arg1);
extern void func_80092F38(Rect63C5C *rect, void *data);
extern void func_80092C8C(s32 arg0);
extern void func_80048DEC(void);
extern void func_800181D4(void *block);

void func_80063C5C(Arg63C5C *arg0)
{
    Rect63C5C rect[2];
    Res63C5C *res;
    s32 s3;
    s32 s0;
    s32 s2;
    u32 s1;
    u32 s6;
    u32 p1;
    u32 p2;
    u32 attr;
    s32 t0;
    s32 t1;

    s3 = arg0->unk1C;
    res = func_8001996C(arg0->unk00, arg0->unk20);
    s0 = s3 - 1;

    s1 = (s0 & 1) << 8;
    s2 = ((s0 >> 1) << 7) + 0x180;

    rect[0].x = s2;
    rect[0].y = s1;
    rect[0].w = 0x80;
    rect[0].h = 0xFC;

    func_80092F38(rect, res->unk28);
    func_80092C8C(0);

    t0 = s0;
    p1 = s1 >> 4;
    p2 = ((s2 & 0x3FF) >> 6) | 0x80;
    s6 = p1 | p2;

    if (s0 < 0) {
        t0 = s3 + 2;
    }
    s2 = ((t0 << 6) & 0x100) + 0x180;

    t1 = s0;
    p1 = s0 & 3;
    if (s0 < 0) {
        t1 = s3 + 6;
    }
    s1 = p1 + ((t1 << 5) | 0xFC);

    rect[1].x = s2;
    rect[1].y = s1;
    rect[1].w = 0x100;
    rect[1].h = 1;

    func_80092F38(&rect[1], res->unk2C + 8);
    func_80092C8C(0);

    if (s0 == 9) {
        func_80048DEC();
    }

    attr = s1 << 6;
    attr |= (s2 >> 4) & 0x3F;

    D_800A8E80[s0] = (attr << 16) | s6;

    func_800181D4(arg0);
}
