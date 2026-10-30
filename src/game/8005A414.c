#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RectA414;

typedef struct {
    u8 unk00[8];
    s32 count;
} HeaderA414;

typedef struct {
    u8 *unk00;
} ArgA414;

extern void **D_8009F7D8;
extern s32 D_8009F7DC;

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800181D4(void *block);
extern void func_8005A220(void);
extern void func_8008CD10(void *dest, void *src, s32 size);
extern void func_80092F38(RectA414 *rect, void *data);
extern void func_80092C8C(s32 arg);
extern s32 func_8005A2BC(void **array, s32 count);

void func_8005A414(ArgA414 *arg0)
{
    register HeaderA414 *header __asm__("$18");
    s32 count;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    s32 size;
    u8 *p;
    RectA414 rect[2];

    header = (HeaderA414 *)arg0->unk00;
    count = header->count;

    if (count <= 0) {
        goto cleanup;
    }

    D_8009F7DC = count;
    D_8009F7D8 = func_80017EBC(count * 4, 2);
    p = (u8 *)header + 0xC;
    if (D_8009F7D8 == 0) {
        goto cleanup;
    }

    for (i = 0; i < count; i++) {
        size = p[2] * 8;
        D_8009F7D8[i] = func_80017EBC(size + 8, 2);
        *(s32 *)D_8009F7D8[i] = *(s32 *)p;
        func_8008CD10((u8 *)D_8009F7D8[i] + 8, p + 4, size);
        p += size + 4;
    }

    p += 8;
    x = 0;
    y = 0;
    for (i = 0; i < count; i++) {
        ((u8 *)D_8009F7D8[i])[4] = x;
        ((u8 *)D_8009F7D8[i])[5] = y;
        for (j = 0; j < ((u8 *)D_8009F7D8[i])[2]; j++) {
            rect[0].x = x + 0x140;
            rect[0].y = y + 0x1E0;
            rect[0].w = 0x10;
            rect[0].h = 1;
            func_80092F38(&rect[0], p);
            func_80092C8C(0);
            x += 0x10;
            p += 0x20;
            if (x == 0x40) {
                x = 0;
                y++;
            }
        }
    }

    rect[1].x = 0x140;
    rect[1].y = 0;
    rect[1].w = 0x40;
    rect[1].h = 0x1E0;
    func_80092F38(&rect[1], p + 8);
    func_80092C8C(0);

    if ((func_8005A2BC(D_8009F7D8, count) & 0xFF) != 0) {
        goto done;
    }

cleanup:
    func_8005A220();

done:
    func_800181D4(arg0);
}
