typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef struct { s32 x,y,z; } Vec;
typedef struct { u32 tag, color; s16 x0,y0,x1,y1,x2,y2,x3,y3; } Packet;
extern s32 D_8009F7E0, D_8009F7E4, D_8009F7E8, D_8009F7EC;
extern Packet *D_8009FA90, *D_8009F9C0;
extern u16 D_800A8EA6;
#define MUL(out,a,b) __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
int func_8005B088(Vec *p, u32 color, int side, int type)
{
    register u32 col __asm__("$13") = color;
    register s32 dz __asm__("$12"), dx __asm__("$11"), az __asm__("$5"), ax __asm__("$10");
    register s32 v0 __asm__("$2"), v1 __asm__("$3");
    register Packet *cur __asm__("$3");
    register s32 dist __asm__("$4"), a __asm__("$3"), b __asm__("$2");
    register s32 yy __asm__("$10"), c __asm__("$4"), s __asm__("$5");
    s32 t;
    register s32 xx __asm__("$8");
    __asm__ volatile("" : "+r"(col));
    v1 = p->z; v0 = D_8009F7E8; dz = v1 - v0;
    v1 = dz >> 31; v0 = v1 + dz; az = v0 ^ v1;
    if (az > 0x25555) return 0;
    v1 = p->x; v0 = D_8009F7EC; dx = v1 - v0;
    v1 = dx >> 31; v0 = v1 + dx; ax = v0 ^ v1;
    if (ax > 0x25555) return 0;
    if (az < ax) { az ^= ax; ax ^= az; az ^= ax; }
    if ((az >> 1) < ax) {
        t = az + ax;
        t = (t << 6) + (t << 3) + (t << 1) + az + (ax << 5);
        ax = az - ax;
        az = t >> 7;
    }
    v0 = ax >> 2; v0 = az + v0; v1 = ax >> 6; dist = v0 - v1;
    v0 = dist > 0x25555; __asm__("" : "=r"(v0) : "0"(v0)); if (v0) return 0;
    s = D_8009F7E0;
    MUL(a,s,dz); c = D_8009F7E4; MUL(b,c,dx);
    yy = a + b; yy = (yy + (yy >> 1)) >> 12;
    MUL(a,c,dz); MUL(b,s,dx);
    xx = a - b; xx = (xx + (xx >> 1)) >> 12;
    if (type == 0) {
        register Packet *q __asm__("$4");
        cur = D_8009FA90;
        if (cur < D_8009F9C0) goto alloc0;
        q = cur; __asm__(""); goto ready0;
alloc0: q = cur; D_8009FA90 = (Packet *)((u8 *)q + 28);
ready0:
        q->tag = 0x04000000; __asm__ volatile("" ::: "memory"); v0 = col | 0x66000000; __asm__("" : "=r"(v0) : "0"(v0)); q->color = v0; __asm__ volatile("" ::: "memory");
        v0 = xx + 53; v1 = side << 1; v0 = (v0 + v1) >> 1; __asm__("" : "=r"(v0) : "0"(v0)); q->x0 = v0; __asm__ volatile("" ::: "memory"); v0 = (yy + 86) >> 1; __asm__("" : "=r"(v0) : "0"(v0)); q->y0 = v0; __asm__ volatile("" ::: "memory");
        q->x2 = 3; q->y2 = 3; __asm__ volatile("" ::: "memory");
        ((u8 *)q)[12] = 128; ((u8 *)q)[13] = 201; __asm__ volatile("" ::: "memory");
        v0 = D_800A8EA6; __asm__("" : "=r"(v0) : "0"(v0)); q->y1 = v0;
    } else if (type == 1) {
        register Packet *q __asm__("$5");
        register int sd __asm__("$4");
        cur = D_8009FA90;
        if (cur < D_8009F9C0) goto alloc1;
        q = cur; __asm__(""); goto ready1;
alloc1: q = cur; D_8009FA90 = (Packet *)((u8 *)q + 28);
ready1:
        q->tag = 0x04000000; __asm__ volatile("" ::: "memory"); v0 = col | 0x20000000; __asm__("" : "=r"(v0) : "0"(v0)); q->color = v0; __asm__ volatile("" ::: "memory");
        v0 = xx + 56; __asm__("" : "=r"(v0) : "0"(v0)); sd = side << 1; v0 = (v0 + sd) >> 1; q->x0 = v0;
        v0 = (yy + 84) >> 1; __asm__("" : "=r"(v0) : "0"(v0)); q->y0 = v0; __asm__ volatile("" ::: "memory");
        v0 = xx + 64; __asm__("" : "=r"(v0) : "0"(v0)); v0 = (v0 + sd) >> 1; v1 = (yy + 92) >> 1; q->x1 = v0; __asm__ volatile("" ::: "memory");
        v0 = xx + 48; __asm__("" : "=r"(v0) : "0"(v0)); v0 = (v0 + sd) >> 1; q->y1 = v1; q->x2 = v0; q->y2 = v1;
    } else {
        register Packet *q __asm__("$7");
        register int sd __asm__("$5"), xp __asm__("$4");
        cur = D_8009FA90;
        if (cur < D_8009F9C0) goto alloc2;
        q = cur; __asm__(""); goto ready2;
alloc2: q = cur; D_8009FA90 = (Packet *)((u8 *)q + 28);
ready2:
        q->tag = 0x05000000; __asm__ volatile("" ::: "memory");
        v0 = col | 0x28000000; __asm__("" : "=r"(v0) : "0"(v0));
        xp = xx + 56; __asm__("" : "=r"(xp) : "0"(xp));
        sd = side << 1; xp = (xp + sd) >> 1; q->color = v0;
        v0 = (yy + 82) >> 1; q->y0 = v0; __asm__ volatile("" ::: "memory");
        v0 = xx + 62; __asm__("" : "=r"(v0) : "0"(v0)); v0 = (v0 + sd) >> 1;
        v1 = (yy + 88) >> 1; q->x1 = v0; __asm__ volatile("" ::: "memory");
        v0 = xx + 50; __asm__("" : "=r"(v0) : "0"(v0)); v0 = (v0 + sd) >> 1; q->x2 = v0;
        v0 = (yy + 94) >> 1; q->x0 = xp; q->y1 = v1; q->y2 = v1; q->x3 = xp; q->y3 = v0;
    }
    return 1;
}
