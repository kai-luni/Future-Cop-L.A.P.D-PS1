// Categories: Graphics > GPU > Debug Validation | Platform > GPU
typedef struct { short x, y, w, h; } Rect;
typedef struct { unsigned char mode, pad; short width, height; } State;
extern State gGpuDebugLevel __asm__("D_8009DDF2");
extern  void (*gLogPrintf)(char *, ...) __asm__("D_8009DDEC");
extern char D_80012A58[], D_80012A64[], D_80012A78[];

/**
 * @brief Validates a rectangle against the GPU library's limits and logs a
 *        complaint when it does not fit.
 *
 * @param name Textual name of the calling operation (used in the message).
 * @param arg Rectangle (x, y, w, h).
 *
 * @return Nothing; a violating rectangle is printed, not corrected - the caller
 *         continues, which is the point of this debug-only check.
 *
 * @par Confirmed
 * The library state at D_8009DDF2 carries a mode byte:
 * - mode 1 runs the full check (w > 0, h > 0, x >= 0, y >= 0, x + w and y + h
 *   inside width/height) and logs D_80012A58 when it fails,
 * - mode 2 logs D_80012A78 unconditionally,
 * - every other mode returns silently.
 * In both logging modes the rectangle is printed with the format at D_80012A64.
 *
 * @par Callers
 * func_80092F38, func_80092F98, func_80092FF8 and func_80093Axx - every transfer
 * wrapper of the GPU layer.
 */
void func_80092CF4(char *name, Rect *arg)
{
    register State *state __asm__("$4");
    char *message;
    register Rect *r __asm__("$16");
    int mode;
    int w, y;
    register int last __asm__("$3");
    register int a __asm__("$5");
    register int b __asm__("$6");
    register int c __asm__("$7");
    register void (*callback)(char *, ...) __asm__("$2");
    __asm__("lui %0,%%hi(D_8009DDF2)\n\taddiu %0,%0,%%lo(D_8009DDF2)" : "=r"(state) : : "$16");
    mode = state->mode;
    r = arg;
    if (mode == 1) goto mode1;
    if (mode == 2) goto mode2;
    return;
mode1:
    __asm__("" : "+r"(r));
    w = r->w;
    __asm__("" : "+r"(w));
    if (state->width < w) goto bad;
    if (state->width < w + r->x) goto bad;
    y = r->y;
    __asm__("" : "+r"(y));
    if (state->height < y) goto bad;
    if (state->height < y + r->h) goto bad;
    if (w <= 0) goto bad;
    if (r->x < 0) goto bad;
    if (y < 0) goto bad;
    if (r->h > 0) return;
bad:
    __asm__("lui %0,%%hi(D_80012A58)" : "=r"(message));
    message += 0x2a58;
    goto print;
mode2:
    __asm__("lui %0,%%hi(D_80012A78)" : "=r"(message));
    message += 0x2a78;
print:
    gLogPrintf(message, name);
    a = r->x;
    b = r->y;
    c = r->w;
    __asm__("" : "+r"(a), "+r"(b), "+r"(c));
    last = r->h;
    __asm__("" : "+r"(last));
    callback = gLogPrintf;
    __asm__("" : "+r"(callback));
    callback(D_80012A64, a, b, c, last);
    __asm__ volatile(".set reorder");
}
