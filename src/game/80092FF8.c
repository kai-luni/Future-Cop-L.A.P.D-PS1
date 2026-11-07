typedef unsigned int u32;
typedef unsigned short u16;
typedef struct { short x,y,w,h; } Rect;
typedef struct { void *pad0; void *pad4; int (*send)(int, void *, int, int); char padC[12]; int arg; } Context;
extern void func_80092CF4(char *, Rect *);
extern char D_80012AAC[];
extern u32 D_8009DE90[];
extern Context *D_8009DDE8;
int func_80092FF8(Rect *r, int a, int b)
{
    register int aa __asm__("$18") = a;
    register int bb __asm__("$17") = b;
    __asm__("" : "+r"(aa), "+r"(bb));
    func_80092CF4(D_80012AAC, r);
    if (!r->w || !r->h) return -1;
    {
        register u32 *base __asm__("$3");
        register u32 val __asm__("$2");
        register u32 lo __asm__("$4");
        register u32 first __asm__("$5");
        register Context *ctx __asm__("$7");
        register int size __asm__("$6");
        val = bb << 16;
        __asm__("" : "+r"(val));
        base = D_8009DE90;
        __asm__("" : "+r"(base));
        lo = aa & 0xffff;
        val = val | lo;
        first = *(u32 *)r;
        ctx = D_8009DDE8;
        __asm__("" : "+r"(val), "+r"(first), "+r"(ctx));
        size = 20;
        __asm__("" : "+r"(size));
        base[1] = val;
        base[0] = first;
        base[2] = *(u32 *)&r->w;
        val = ctx->send(ctx->arg, base - 2, size, 0);
        __asm__ volatile(".set reorder");
        return val;
    }
}
