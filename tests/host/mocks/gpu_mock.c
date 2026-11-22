/* Shared GPU transfer stand-ins. See gpu_mock.h for the contract. */
#include "gpu_mock.h"
#include "fcm_report.h"

struct FcmGpuMock fcm_gpu;

void fcm_gpu_reset(void)
{
    u8 *p = (u8 *)&fcm_gpu;
    s32 i;
    for (i = 0; i < (s32)sizeof(fcm_gpu); i++) p[i] = 0;
    fcm_gpu.capture_rect = 1;
    /* copy_bytes stays 0: the mock does not know the payload length, and
     * guessing it would read past the caller's buffer. */
}

/* func_80092F38 gLoadImage(void *rect, void *data) -> void
 *
 * Retail: hands a rectangle plus its source to the VRAM transfer. The
 * declaration is (void *, void *) because the retail callers disagree on the
 * rectangle type (Rect, s16 *, Local *, ...) - they are all one pointer word,
 * which is what the ABI actually passes. */
void gLoadImage(void *rect, void *data) __asm__("func_80092F38");
void gLoadImage(void *rect, void *data)
{
    struct FcmGpuLoad *rec;
    s32 i, n;

    if (fcm_gpu.loads >= FCM_GPU_LOG_MAX) {
        fcm_gpu.load_overflow = 1;
        return;                       /* still counted below, so the test sees it */
    }
    rec = &fcm_gpu.load[fcm_gpu.loads];
    for (i = 0; i < (s32)sizeof(*rec); i++) ((u8 *)rec)[i] = 0;
    rec->data = (s32)(unsigned long)data;

    if (fcm_gpu.capture_rect && rect) {
        rec->rect.x = ((struct FcmGpuRect *)rect)->x;
        rec->rect.y = ((struct FcmGpuRect *)rect)->y;
        rec->rect.w = ((struct FcmGpuRect *)rect)->w;
        rec->rect.h = ((struct FcmGpuRect *)rect)->h;
    }

    n = fcm_gpu.copy_bytes;
    if (n > FCM_GPU_COPY_MAX) n = FCM_GPU_COPY_MAX;
    if (data && n > 0) {
        u8 *src = (u8 *)data;
        for (i = 0; i < n; i++) rec->bytes[i] = src[i];
        rec->copied = n;
    }
    fcm_gpu.loads++;
}

/* func_80092C8C gDrawSync(s32 mode) -> void
 *
 * Retail: waits for (0) or flushes the GPU. The GP0/GP1 state it touches is
 * hardware; the only observable contract a host test can hold the unit to is
 * "one sync per transfer, with the mode the source says". */
void gDrawSync(s32 mode) __asm__("func_80092C8C");
void gDrawSync(s32 mode)
{
    if (fcm_gpu.syncs < FCM_GPU_LOG_MAX)
        fcm_gpu.sync_modes[fcm_gpu.syncs] = mode;
    else
        fcm_gpu.sync_overflow = 1;
    fcm_gpu.sync_last_mode = mode;
    fcm_gpu.syncs++;
}
