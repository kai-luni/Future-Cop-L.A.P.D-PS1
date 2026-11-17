// Categories: Game Engine > Request Queue
typedef struct Request {
    int a, b, c;
    void (*callback)(int);
    int context, mode;
} Request;
extern Request gPendingEntryList[] __asm__("D_800A8EA8");
extern int D_8009F998, D_8009FA80, D_8009FAD0, D_8009FA34;
extern void func_80021490(Request *);

/**
 *  @brief Copies a request into a nine-slot circular queue.
 *
 *  Stores all six request fields at the current write index and starts
 *  processing the original request when the write and comparison indices match.
 *  @param req Request whose fields are copied and which may be dispatched.
 *  @par Globals
 *  D_800A8EA8 holds queued requests; D_8009F998 is the write index.
 *  D_8009FA80 is compared with that index before it advances.
 *  @par Control flow
 *  Conditionally dispatches, increments the write index, and wraps it at nine.
 *  @par Calls
 *  func_80021490 receives req when the two indices match.
 *  @par Side effects
 *  Overwrites a queue slot and updates the write index; the callee may have further effects.
 *  @par Callers
 *  Confirmed: func_80021898, func_80022D84, func_80023CA4.
 */
void func_800212F8(Request *req)
{
    register Request *dst __asm__("$2") = &gPendingEntryList[D_8009F998];
    dst->a = req->a;
    dst->b = req->b;
    dst->c = req->c;
    dst->callback = req->callback;
    dst->context = req->context;
    dst->mode = req->mode;
    if (D_8009F998 == D_8009FA80)
        func_80021490(req);
    D_8009F998++;
    if (D_8009F998 >= 9)
        D_8009F998 = 0;
}

