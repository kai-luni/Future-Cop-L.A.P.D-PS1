// Categories: Graphics > Video Decode Output | Graphics > VRAM Transfer
#include "game/types.h"

typedef struct { u16 x, y, w, h; } Rect;
typedef struct { s16 x, y, w, h; } Bounds;
typedef struct {
    Bounds rects[2];
    volatile s32 index;
    u8 pad14[8];
    volatile Rect rect;
    s32 dest;
    u8 pad28[0xF4];
    volatile s32 flag;
} State;

extern State *gDecodeJob __asm__("D_8009F4BC");
extern s32 gStripScaleEnable __asm__("D_8009F4C0");
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;
extern void func_800847E0(s32, s32, Rect *);
extern void func_80092F38(Rect *, s32);
extern void func_80096070(s32, s32);

/**
 * @brief Frame-complete callback of the video decoder: writes one decoded
 *        strip into the frame buffer and advances to the next one.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. If D_8009F4C0 == 1 and D_8009F4C4 != 0 the strip is scaled horizontally to
 *    2/3 (x, w divided by three and multiplied by two) into a local rectangle,
 *    moved in place with func_800847E0 and sent with func_80092F38; otherwise
 *    the job's own rectangle is sent unchanged.
 * 2. The strip cursor advances by D_8009F4C8 * 8 pixels.
 * 3. As long as the cursor stays inside the current destination rectangle the
 *    work buffer is cleared with func_80096070(w * h / 2) - the /2 is the
 *    16-bit pixel size - so the next strip can be decoded into it.
 * 4. Otherwise the frame is finished: flag = 1 (the poll in func_800861E4 sees
 *    it), the destination index flips (double buffer) and the rectangle is
 *    reset to the other buffer's origin.
 *
 * @par Globals
 * D_8009F4BC (decode job: rects[2], index, rect, dest, flag), D_8009F4C0
 * (scaling mode), D_8009F4C4 (scale enable), D_8009F4C8 (column step).
 *
 * @par Callers
 * Installed as the frame callback by func_800861E4 (via func_8009612C), so the
 * decoder calls it once per strip.
 */
void func_80085074(void)
{
    Rect rect;
    State *s;
    if (gStripScaleEnable == 1 && D_8009F4C4 != 0) {
        rect.x = ((s16)gDecodeJob->rect.x * 2) / 3;
        rect.y = gDecodeJob->rect.y;
        rect.w = ((s16)gDecodeJob->rect.w * 2) / 3;
        rect.h = gDecodeJob->rect.h;
        func_800847E0(gDecodeJob->dest, gDecodeJob->dest, &rect);
        func_80092F38(&rect, gDecodeJob->dest);
    } else {
        func_80092F38(&gDecodeJob->rect, gDecodeJob->dest);
    }
    s = gDecodeJob;
    s->rect.x += D_8009F4C8 * 8;
    if ((s16)s->rect.x < (s16)s->rects[s->index].x + (s16)s->rects[s->index].w) {
        func_80096070(s->dest, ((s16)s->rect.w * (s16)s->rect.h) / 2);
    } else {
        s->flag = 1;
        s->index = !s->index;
        s->rect.x = s->rects[s->index].x;
        s->rect.y = s->rects[s->index].y;
    }
}
