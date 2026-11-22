/* Shared stand-ins for the PS1 GPU transfer layer (retail labels
 * func_80092F38 LoadImage and func_80092C8C DrawSync).
 *
 * Both are leaves in retail: they hand a rectangle to the GPU library and
 * return. Nothing about them is game logic, so every harness that needs a unit
 * which uploads a texture used to write its own no-op - and a no-op cannot
 * tell you *what* the unit uploaded. This mock records instead: the rectangle,
 * the first bytes of the payload, and the sync mode, so a test can assert the
 * exact transfer the retail code describes in its doc block.
 *
 * Contract:
 *
 *  1. Bound to the retail label with an asm alias, so a wrong binding is a
 *     link error. Signatures are the retail ones: (rect, data) and (mode).
 *  2. Nothing is invented. The recorded payload is a copy of the bytes the
 *     unit actually passed; a field nobody wrote stays 0.
 *  3. Do not link this object together with tests/host/stubs.c: that file also
 *     defines both labels (as silent no-ops), and the link would rightly fail
 *     with a duplicate symbol.
 *  4. Both labels *are* supported, so no call can arrive that this mock cannot
 *     answer. The rectangle is read as the s16 x,y,w,h Rect layout the
 *     texture-upload path uses; a unit that passes some other layout still
 *     gets counted, it just records meaningless geometry - which is why
 *     `capture_rect` exists to turn the read off.
 */
#ifndef FCM_GPU_MOCK_H
#define FCM_GPU_MOCK_H

#include "fcm_types.h"

#define FCM_GPU_LOG_MAX 16
#define FCM_GPU_COPY_MAX 32

struct FcmGpuRect {
    s16 x, y, w, h;
};

struct FcmGpuLoad {
    struct FcmGpuRect rect;
    s32 data;                        /* the pointer the unit passed */
    s32 copied;                      /* bytes captured below (0 = no data ptr) */
    u8 bytes[FCM_GPU_COPY_MAX];
};

struct FcmGpuMock {
    s32 capture_rect;                /* 1 = read the rect, 0 = leave it zero */
    s32 copy_bytes;                  /* bytes per load to shadow, <= COPY_MAX */

    s32 loads;
    struct FcmGpuLoad load[FCM_GPU_LOG_MAX];
    s32 load_overflow;               /* 1 = more loads than the log holds */

    s32 syncs;
    s32 sync_last_mode;
    s32 sync_modes[FCM_GPU_LOG_MAX];
    s32 sync_overflow;
};

extern struct FcmGpuMock fcm_gpu;

/* Zero the struct and restore the defaults.
 *
 * `capture_rect` is on because reading the four s16 of a Rect is what the
 * transfer path promises. `copy_bytes` is 0 (off), because the mock cannot know
 * how large the caller's payload is: a non-zero default would read past the end
 * of whatever the unit passed. A test that wants the payload shadowed sets
 * copy_bytes to the number of bytes it knows are valid. */
void fcm_gpu_reset(void);

/* The stand-ins, each bound to its retail label. `void *` for the rectangle
 * because the retail callers disagree on its type (Rect, s16 *, ...) - they are
 * all one pointer word, which is what the ABI passes. */
void gLoadImage(void *rect, void *data) __asm__("func_80092F38");
void gDrawSync(s32 mode) __asm__("func_80092C8C");

#endif /* FCM_GPU_MOCK_H */
