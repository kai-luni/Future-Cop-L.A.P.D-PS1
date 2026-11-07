typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
struct Rect { s16 x, y, w, h; };
struct Packet { u8 pad[3]; u8 len; u32 cmd, mode, xy, pos, wh; };
void func_80092660(struct Packet *p, struct Rect *r, int x, int y)
{
    register struct Packet *out __asm__("$8") = p;
    register int len __asm__("$4");
    u32 xy;
    len = 5;
    if (!r->w || !r->h) len = 0;
    out->cmd = 0x01000000;
    out->mode = 0x80000000;
    out->len = len;
    xy = *(u32 *)r;
    out->pos = (y << 16) | (x & 0xffff);
    out->xy = xy;
    out->wh = *(u32 *)&r->w;
}
