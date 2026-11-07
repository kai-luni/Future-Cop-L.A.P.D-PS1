typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    union { u32 word; u8 bytes[4]; } hdr;
    u32 code;
} Page;

typedef struct {
    union { u32 word; u8 bytes[4]; } hdr;
    u8 r, g, b, code;
    short x, y;
    u8 u, v;
    u16 pad, w, h;
} Sprt;

extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;

void func_8002549C(int value) {
    u8 *packet;
    register int arg __asm__("$7") = value;

    if (D_8009FA90 < D_8009F9C0) goto p1_alloc;
    __asm__("");
    packet = D_8009FA90; goto p1;
p1_alloc:
    packet = D_8009FA90; D_8009FA90 = packet + 28;
p1:
    {
        register u32 mask __asm__("$4") = 0x00FFFFFF;
        ((Page *)packet)->hdr.word = (((Page *)packet)->hdr.word & 0xFF000000) | (arg & mask);
        ((Page *)packet)->hdr.bytes[3] = 1;
        ((Page *)packet)->code = 0xe1000105;
    }

    if (D_8009FA90 < D_8009F9C0) goto s1_alloc;
    __asm__("");
    packet = D_8009FA90; goto s1;
s1_alloc:
    packet = D_8009FA90; D_8009FA90 = packet + 28;
s1:
    {
        register u32 mask __asm__("$4") = 0x00FFFFFF;
        register int h __asm__("$2");
        ((Sprt *)packet)->code = 0x64;
        ((Sprt *)packet)->r = 0x80;
        ((Sprt *)packet)->g = 0x80;
        ((Sprt *)packet)->b = 0x80;
        ((Sprt *)packet)->w = 0xa0;
        ((Sprt *)packet)->hdr.word = (((Sprt *)packet)->hdr.word & 0xFF000000) | (arg & mask);
        ((Sprt *)packet)->u = 0;
        ((Sprt *)packet)->v = 0;
        ((Sprt *)packet)->x = 0;
        ((Sprt *)packet)->y = 0;
        ((Sprt *)packet)->hdr.bytes[3] = 4;
        h = 0xf0;
        ((Sprt *)packet)->h = h;
    }

    if (D_8009FA90 < D_8009F9C0) goto p2_alloc;
    __asm__("");
    packet = D_8009FA90; goto p2;
p2_alloc:
    packet = D_8009FA90; D_8009FA90 = packet + 28;
p2:
    {
        register u32 mask __asm__("$4") = 0x00FFFFFF;
        ((Page *)packet)->hdr.word = (((Page *)packet)->hdr.word & 0xFF000000) | (arg & mask);
        ((Page *)packet)->hdr.bytes[3] = 1;
        ((Page *)packet)->code = 0xe1000115;
    }

    if (D_8009FA90 < D_8009F9C0) goto s2_alloc;
    __asm__("");
    packet = D_8009FA90; goto s2;
s2_alloc:
    packet = D_8009FA90; D_8009FA90 = packet + 28;
s2:
    {
        register u32 mask __asm__("$4") = 0x00FFFFFF;
        ((Sprt *)packet)->code = 0x64;
        ((Sprt *)packet)->r = 0x80;
        ((Sprt *)packet)->w = 0xa0;
        ((Sprt *)packet)->g = 0x80;
        ((Sprt *)packet)->b = 0x80;
        ((Sprt *)packet)->x = 0xa0;
        ((Sprt *)packet)->h = 0xf0;
        ((Sprt *)packet)->hdr.word = (((Sprt *)packet)->hdr.word & 0xFF000000) | (arg & mask);
        ((Sprt *)packet)->u = 0;
        ((Sprt *)packet)->v = 0;
        ((Sprt *)packet)->y = 0;
        ((Sprt *)packet)->hdr.bytes[3] = 4;
    }
}
