typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
struct Packet { u32 addr:24; u32 len:8; u8 r,g,b,code; short x,y; u8 u,v; u16 clut,w,h; };
extern u8 *D_8009FA90, *D_8009F9C0;
extern u32 D_800A8EA4[], D_8009FAA4;
void func_8006BF44(int a0, int a1, int a2, int a3)
{
    u8 *packet;
    u8 *cur2;
    u8 *lim2;
    u32 page;
    register int u __asm__("$12");
    register int v __asm__("$11");
    register u32 texture __asm__("$10");
    register u32 clut __asm__("$2");
    u = 0xA6;
    if (a0 == -1) { u = 0x96; v = 0xBC; }
    else { register int tmp __asm__("$2") = a0 << 4; __asm__("" : "=r"(tmp) : "0"(tmp)); v = tmp + 0x9C; }
    if (D_8009FA90 < D_8009F9C0) goto page_alloc;
    __asm__("");
    packet = D_8009FA90;
    goto page_ready;
page_alloc:
    packet = D_8009FA90;
    D_8009FA90 = packet + 0x1C;
page_ready:
    ((struct Packet *)packet)->addr = 0;
    texture = D_800A8EA4[0];
    ((struct Packet *)packet)->len = 1;
    page = (texture & 0x9FF) | 0xE1000600;
    cur2 = D_8009FA90;
    lim2 = D_8009F9C0;
    *(u32 *)(packet + 4) = page;
    if (cur2 < lim2) goto sprt_alloc;
    __asm__("");
    packet = cur2;
    goto sprt_ready;
sprt_alloc:
    packet = cur2;
    D_8009FA90 = packet + 0x1C;
sprt_ready:
    ((struct Packet *)packet)->code = 0x66;
    ((struct Packet *)packet)->addr = 0;
    ((struct Packet *)packet)->len = 4;
    if (a1 && (D_8009FAA4 & 0x40)) {
        ((struct Packet *)packet)->r = 0xC0;
        ((struct Packet *)packet)->g = 0xA0;
        ((struct Packet *)packet)->b = 0xA0;
    } else {
        ((struct Packet *)packet)->r = 0x60;
        ((struct Packet *)packet)->g = 0x60;
        ((struct Packet *)packet)->b = 0x60;
    }
    ((struct Packet *)packet)->y = a3 - 16;
    ((struct Packet *)packet)->w = 16;
    ((struct Packet *)packet)->h = 16;
    __asm__ volatile("" ::: "memory");
    clut = texture >> 16;
    __asm__("" : "=r"(clut) : "0"(clut));
    ((struct Packet *)packet)->x = a2;
    ((struct Packet *)packet)->u = u;
    ((struct Packet *)packet)->v = v;
    ((struct Packet *)packet)->clut = clut;
}
