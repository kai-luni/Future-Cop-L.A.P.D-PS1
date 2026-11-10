typedef unsigned int u32;
typedef unsigned char u8;
typedef signed short s16;

struct Rect { s16 x, y, w, h; };
struct Packet { u8 pad[3]; u8 len; u32 cmd, xy, wh; };

void func_800926C0(struct Packet *p, struct Rect *r)
{
    int size = (r->w * r->h + 1) / 2;
    int len = size + 4;
    if ((unsigned int)size >= 13)
        len = 0;
    p->len = len;
    p->cmd = 0xA0000000;
    p->xy = *(u32 *)r;
    p->wh = *(u32 *)&r->w;
    *(u32 *)((u32)p + ((u32)len << 2)) = 0x01000000;
}
