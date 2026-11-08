typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct { int x,y,z; } Vec;
typedef struct { short x,y,z,pad; } SVec;
typedef struct { u8 pad[0x5c]; Vec pos; } Target;
typedef struct { u8 pad[0x6c]; Target *target; } State;
typedef struct { u8 pad[0x24]; int flags; u8 pad2[0x84]; State *state; u8 pad3[0x2c]; u8 *model; } Object;
typedef struct { u8 pad[0x7c]; Vec pos; } Camera;
typedef struct { u32 addr:24; u32 len:8; u32 code; } Page;
typedef struct { u32 addr:24; u32 len:8; u8 r,g,b,code; short x,y; u8 u,v; u16 clut,w,h; } Sprt;
typedef struct { u32 addr:24; u32 len:8; u8 r,g,b,code; short x0,y0,x1,y1; } Line;
extern Camera *D_8009F9E4;
extern u8 *D_8009FA90, *D_8009F9C0;
extern u32 D_800A8EA0[];
extern void func_80017668(void *, int, Vec *);
extern int func_800684AC(Vec *, Vec *, Vec *, int, int);
extern void func_80010820(void *);
extern void func_8001047C(SVec *, SVec *, int *, int);
void func_80047888(Object *obj)
{
    Vec points[3];
    SVec screen[3];
    int flag;
    int i;
    u8 *packet;
    u32 tex;
    Target *target = obj->state->target;
    if (!target || (obj->flags & 0x1000000)) return;
    points[2] = target->pos;
    func_80017668(obj->model + 0x28, 0, &points[0]);
    if (!func_800684AC(&points[0], &points[2], &points[1], 0, 1)) points[1] = points[2];
    for (i = 0; i < 3; i++) {
        screen[i].x = (points[i].x - D_8009F9E4->pos.x) >> 4;
        screen[i].y = (points[i].y - D_8009F9E4->pos.y) >> 4;
        screen[i].z = (points[i].z - D_8009F9E4->pos.z) >> 4;
    }
    func_80010820((u8 *)D_8009F9E4 + 0x28);
    func_8001047C(screen, screen, &flag, 3);
    if (((u8 *)&flag)[2] & 1) return;
    if (D_8009FA90 < D_8009F9C0) goto a1;
    __asm__(""); packet = D_8009FA90; goto p1;
a1: packet = D_8009FA90; D_8009FA90 = packet + 28;
p1:
    { register u32 mask __asm__("$3") = 0xff000000;
      __asm__("" : "=r"(mask) : "0"(mask));
      *(u32 *)packet &= mask; }
    tex = D_800A8EA0[0];
    ((Page *)packet)->len = 1;
    ((Page *)packet)->code = (tex & 0x9ff) | 0xe1000000;
    if (D_8009FA90 < D_8009F9C0) goto a2;
    __asm__(""); packet = D_8009FA90; goto p2;
a2: packet = D_8009FA90; D_8009FA90 = packet + 28;
p2:
    ((Sprt *)packet)->code = 0x66;
    ((Sprt *)packet)->clut = tex >> 16;
    ((Sprt *)packet)->r = 0x80;
    ((Sprt *)packet)->g = 0x80;
    ((Sprt *)packet)->b = 0x80;
    ((Sprt *)packet)->v = 0xd2;
    ((Sprt *)packet)->w = 0x10;
    ((Sprt *)packet)->h = 0x10;
    ((Sprt *)packet)->u = 0;
    ((Sprt *)packet)->addr = 0;
    ((Sprt *)packet)->len = 4;
    ((Sprt *)packet)->x = screen[2].x - 8;
    ((Sprt *)packet)->y = screen[2].y - 8;
    { register u8 *p __asm__("$5");
    register int depth __asm__("$7");
    register int z __asm__("$4");
    register int t __asm__("$2");
    register int m __asm__("$3");
    z = screen[0].z;
    t = z - 0x400;
    __asm__("" : "=r"(t) : "0"(t));
    m = t >> 31;
    m &= t;
    __asm__("" : "=r"(m) : "0"(m));
    depth = z - m;
    p = D_8009FA90;
    if (p < D_8009F9C0) D_8009FA90 = p + 28;
    { register u32 code __asm__("$3") = 0xe10006a0;
    register u32 low __asm__("$4") = 0xffffff;
    register u32 one __asm__("$2") = 1;
    register u8 *next __asm__("$6");
    register u32 hdr __asm__("$3");
    register u32 mask __asm__("$2");
    __asm__("" : "=r"(code) : "0"(code));
    __asm__("" : "=r"(one) : "0"(one));
    ((volatile u8 *)p)[3] = one;
    mask = 0xff000000;
    low &= depth;
    ((volatile Page *)p)->code = code;
    hdr = *(volatile u32 *)p;
    next = D_8009FA90;
    __asm__("" : "=r"(next) : "0"(next));
    hdr &= mask;
    { register u8 *end __asm__("$2") = D_8009F9C0;
    __asm__("" : "=r"(end) : "0"(end));
    *(u32 *)p = hdr | low;
    if (next < end) goto a3; }
    __asm__(""); p = next; goto p3;
a3: p = next; D_8009FA90 = p + 28;
    }
p3:
    ((Line *)p)->code = 0x42;
    ((Line *)p)->r = 0x30;
    ((Line *)p)->g = 0;
    ((Line *)p)->b = 0;
    { register u32 mask __asm__("$4") = 0xffffff;
      *(u32 *)p = (*(u32 *)p & 0xff000000) | (depth & mask); }
    ((Line *)p)->len = 3;
    ((Line *)p)->x0 = screen[0].x;
    ((Line *)p)->y0 = screen[0].y;
    ((Line *)p)->x1 = screen[1].x;
    ((Line *)p)->y1 = screen[1].y;
    }
}
