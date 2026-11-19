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

extern State *gMovieSliceContext __asm__("D_8009F4BC");
extern s32 gStripScaleEnable __asm__("D_8009F4C0");
extern s32 gStripDitherEnabled __asm__("D_8009F4C4");
extern s32 gStripStepWidth __asm__("D_8009F4C8");
extern void func_800847E0(s32, s32, Rect *);
extern void gLoadImage(Rect *, s32) __asm__("func_80092F38");
extern void gStartMdecOutDma(s32, s32) __asm__("func_80096070");
void gUploadMovieStrip(void) __asm__("func_80085074");


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
 * gMovieSliceContext (decode job: rects[2], index, rect, dest, flag), D_8009F4C0
 * (scaling mode), D_8009F4C4 (scale enable), D_8009F4C8 (column step).
 *
 * @par Callers
 * Installed as the frame callback by func_800861E4 (via func_8009612C), so the
 * decoder calls it once per strip.
 *
 * @par Environment path
 * Video draw path, run once per decoded strip: when gStripScaleMode == 1 and
 * gStripScaleEnable != 0 the strip rectangle is narrowed to two thirds in x
 * and w, compacted with func_800847E0 and uploaded with func_80092F38,
 * otherwise the context rectangle is uploaded unchanged to VRAM. The strip x
 * then advances by gStripStepWidth * 8; while it stays inside the current
 * destination rectangle func_80096070 starts the next decode-out transfer of
 * w * h / 2 words into the context buffer, else the frame is flagged complete
 * (flag = 1, polled by func_800861E4), the double-buffer index flips and the
 * rectangle resets to the other buffer origin. Installed by func_800861E4 and
 * func_80085C4C through func_8009612C.
 */
void gUploadMovieStrip(void)
{
    Rect rect;
    State *s;
    if (gStripScaleEnable == 1 && gStripDitherEnabled != 0) {
        rect.x = ((s16)gMovieSliceContext->rect.x * 2) / 3;
        rect.y = gMovieSliceContext->rect.y;
        rect.w = ((s16)gMovieSliceContext->rect.w * 2) / 3;
        rect.h = gMovieSliceContext->rect.h;
        func_800847E0(gMovieSliceContext->dest, gMovieSliceContext->dest, &rect);
        gLoadImage(&rect, gMovieSliceContext->dest);
    } else {
        gLoadImage(&gMovieSliceContext->rect, gMovieSliceContext->dest);
    }
    s = gMovieSliceContext;
    s->rect.x += gStripStepWidth * 8;
    if ((s16)s->rect.x < (s16)s->rects[s->index].x + (s16)s->rects[s->index].w) {
        gStartMdecOutDma(s->dest, ((s16)s->rect.w * (s16)s->rect.h) / 2);
    } else {
        s->flag = 1;
        s->index = !s->index;
        s->rect.x = s->rects[s->index].x;
        s->rect.y = s->rects[s->index].y;
    }
}
