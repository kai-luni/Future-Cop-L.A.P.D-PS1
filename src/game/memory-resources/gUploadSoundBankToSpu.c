// Categories: Resources > Requests
typedef struct { int pad[3]; int size; } Input;
typedef struct { int handle; int size; int arg; void (*callback)(); int context; int flag; } Request;
extern int gAllocSpuRamBlock(int) __asm__("func_80097B28");
extern void gEnqueueSpuTransfer(Request *) __asm__("func_800212F8");
extern void gSignalSpuUploadComplete(void) __asm__("func_80021888");
extern volatile int gSpuUploadCompleted __asm__("D_8009F628");

int gUploadSoundBankToSpu(Input *input, int arg, void (*callback)(), int context) __asm__("func_80021898");

/**
 * @brief Allocates SPU RAM and submits a sound bank upload, waiting when no
 * callback is supplied.
 *
 * @param input Descriptor containing the upload size at +0x0C.
 * @param arg Source address word copied to the transfer request.
 * @param callback Optional completion callback.
 * @param context Value passed to the supplied callback.
 * @return SPU allocation handle after asynchronous submission or observed
 *     completion, -1 on allocation failure, or zero on timeout.
 *
 * @par Confirmed mechanics
 * Loads the descriptor size at +0x0C and rounds it to 64 bytes for allocation.
 * Queues a six-word request with the original size and mode 1. If the callback
 * is null, installs gSignalSpuUploadComplete, clears the completion word, and
 * polls it with a 0xFFFFF countdown.
 *
 * @par Globals
 * gSpuUploadCompleted.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021CB0.
 *
 * @par Role and path
 * The sound bank registration path uses the returned SPU allocation for the
 * bank's data.
 *
 * @par Unknown
 * Timeout returns zero while the original request can still be pending.
 */
int gUploadSoundBankToSpu(Input *input, int arg, void (*callback)(), int context)
{
    int size = input->size;
    int result = 0;
    int timeout = 0xFFFFF;
    int handle;
    Request req;
    handle = gAllocSpuRamBlock(((size + 63) >> 6) << 6);
    if (handle < 0) {
        result = -1;
    } else {
        req.handle = handle;
        req.size = size;
        req.arg = arg;
        req.flag = 1;
        if (callback) {
            req.callback = callback;
            req.context = context;
        } else {
            req.callback = gSignalSpuUploadComplete;
            gSpuUploadCompleted = 0;
        }
        gEnqueueSpuTransfer(&req);
        if (!callback) {
            while (!gSpuUploadCompleted) {
                if (timeout-- <= 0)
                    return result;
            }
        }
        result = handle;
    }
    return result;
}
