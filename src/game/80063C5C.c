// Categories: Graphics > 2D Drawing > Images | Resources > Memory Management
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

extern u32 gTexpageBaseTable[] __asm__("D_800A8E80");
extern Res63C5C *func_8001996C(void *arg0, s32 arg1);
extern void func_80092F38(Rect63C5C *rect, void *data);
extern void func_80092C8C(s32 arg0);
extern void func_80048DEC(void);
extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Blits a decoded image resource into the frame buffer and releases it.
 *
 * @param arg0 Resource descriptor: the resource id at +0, the frame index at +0x1C
 *             and the caller's argument at +0x20.
 * @return Nothing.
 *
 * @par Confirmed
 * The frame index selects the destination: s1 = (index - 1 & 1) << 8 (the two
 * 256-pixel halves of the buffer) and s2 = ((index - 1) >> 1) * 0x80 + 0x180
 * (the vertical stack). Two rectangles of 0x80 x 0xFC are built and sent with
 * func_80092F38, func_80092C8C synchronises, func_80048DEC refreshes the font and
 * func_800181D4 frees the resource.
 *
 * @par Likely role
 * [Superseded] The "show the next video/image frame" step: the resource holds a sequence of
 * 128x252 pictures that are composited into the two page halves.
 * (Correction from source: dispatched on "Cbmp" `0x43626D70` by func_80024EC8,
 * it uploads a 0x80 x 0xFC texture page and a 0x100 x 1 CLUT into VRAM and
 * registers the packed tpage/CLUT word in D_800A8E80[s0] for 3D rendering.)
 *
 * @par Callers
 * [Superseded] The image/video playback path (same family as func_800861E4).
 * Confirmed: func_80024EC8 (asset loader dispatch on tag `0x43626D70` / "Cbmp").
 *
 * @par Texture load path
 * Called by func_80024EC8 on tag `0x43626D70` ("Cbmp"). Unpacks
 * the container with func_8001996C, uploads the 0x80 x 0xFC pixel rectangle
 * (unk28) and 0x100 x 1 CLUT (unk2C + 8) via func_80092F38 / func_80092C8C,
 * refreshes the font via func_80048DEC when `s0 == 9`, stores the packed
 * tpage/CLUT entry in D_800A8E80[s0] (read by func_800131A0), and frees `arg0`
 * via func_800181D4.
 */
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

    gTexpageBaseTable[s0] = (attr << 16) | s6;

    gFreeBlock(arg0);
}
