// Categories: Game Engine > Request Queue
typedef struct Request {
    int a, b, c;
    void (*callback)(int);
    int context, mode;
} Request;
extern Request D_800A8EA8[];
extern int D_8009F998, D_8009FA80, D_8009FAD0, D_8009FA34;
extern void func_80021490(Request *);

/**
 *  @brief Complete the current queued request and start the next one if present.
 *
 *  Copies the current request before invoking its optional callback, advances
 *  the nine-slot ring, and either dispatches the next request or clears a flag.
 *  @par Globals
 *  D_800A8EA8 holds requests; D_8009FA80 and D_8009F998 are ring indices.
 *  D_8009FAD0 guards processing; D_8009FA34 is cleared when the queue empties.
 *  @par Control flow
 *  Returns if guarded or empty; otherwise wraps the index at nine and branches
 *  on whether another request remains.
 *  @par Calls
 *  The copied request callback receives its context when present;
 *  func_80021490 starts processing the next queued request.
 *  @par Side effects
 *  Invokes callbacks, advances the read index, and updates guard and queue flags.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8002139C(void)
{
    Request r;
    if (D_8009FAD0)
        return;
    if (D_8009FA80 == D_8009F998)
        return;
    r = D_800A8EA8[D_8009FA80];
    D_8009FAD0 = 1;
    if (r.callback)
        r.callback(r.context);
    D_8009FA80++;
    if (D_8009FA80 >= 9)
        D_8009FA80 = 0;
    if (D_8009F998 != D_8009FA80)
        func_80021490(&D_800A8EA8[D_8009FA80]);
    else
        D_8009FA34 = 0;
    D_8009FAD0 = 0;
}
