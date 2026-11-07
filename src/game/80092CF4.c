typedef struct { short x, y, w, h; } Rect;
typedef struct { unsigned char mode, pad; short width, height; } State;
extern State D_8009DDF2;
extern void (*D_8009DDEC)(char *, ...);
extern char D_80012A58[], D_80012A64[], D_80012A78[];

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
    D_8009DDEC(message, name);
    a = r->x;
    b = r->y;
    c = r->w;
    __asm__("" : "+r"(a), "+r"(b), "+r"(c));
    last = r->h;
    __asm__("" : "+r"(last));
    callback = D_8009DDEC;
    __asm__("" : "+r"(callback));
    callback(D_80012A64, a, b, c, last);
    __asm__ volatile(".set reorder");
}
