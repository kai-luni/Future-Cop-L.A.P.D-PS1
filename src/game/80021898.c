// Categories: Resources > Requests
typedef struct { int pad[3]; int size; } Input;
typedef struct { int handle; int size; int arg; void (*callback)(); int context; int flag; } Request;
extern int func_80097B28(int);
extern void gEnqueueSpuTransfer(Request *) __asm__("func_800212F8");
extern void func_80021888(void);
extern volatile int D_8009F628;

/**
 *  @brief Allocates a handle and submits a request, optionally waiting for completion.
 *
 *  The allocation size is rounded up to a 64-byte boundary, while the request
 *  retains the original size. A null callback selects the default completion path.
 *  @param input Input descriptor containing the requested size.
 *  @param arg Value passed through to the request.
 *  @param callback Completion callback, or null to wait for the completion flag.
 *  @param context Context passed through when callback is non-null.
 *  @return The handle on completion or asynchronous submission, -1 on allocation failure, or 0 on timeout.
 *  @par Globals
 *  D_8009F628 is cleared and polled for the default completion path.
 *  @par Control flow
 *  Allocation failure returns -1; otherwise a null callback causes a bounded polling loop.
 *  @par Calls
 *  func_80097B28 allocates the handle; gEnqueueSpuTransfer submits the request.
 *  @par Side effects
 *  Submits a request and may clear the completion flag; timeout does not cancel the request.
 *  @par Callers
 *  Confirmed: func_80021CB0.
 */
int func_80021898(Input *input, int arg, void (*callback)(), int context)
{
    int size = input->size;
    int result = 0;
    int timeout = 0xFFFFF;
    int handle;
    Request req;
    handle = func_80097B28(((size + 63) >> 6) << 6);
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
            req.callback = func_80021888;
            D_8009F628 = 0;
        }
        gEnqueueSpuTransfer(&req);
        if (!callback) {
            while (!D_8009F628) {
                if (timeout-- <= 0)
                    return result;
            }
        }
        result = handle;
    }
    return result;
}
