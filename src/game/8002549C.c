// Categories: Graphics > Display Setup | Graphics > GPU Primitives
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

extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");

/**
 * @brief Emits the display set-up for one screen: two texpage/tile words and
 *        two 160x240 sprites that together cover the 320x240 frame buffer.
 *
 * @param value The 24-bit ordering-table link tag written into all four
 *              primitives (only the low 24 bits are used; the top byte carries
 *              the primitive size code).
 *
 * @return Nothing. Four primitives of 28 bytes are taken from the 0x1C pool
 *         (D_8009FA90, limited by D_8009F9C0); when the pool is full the
 *         primitives are written to the last slot instead of advancing the
 *         cursor.
 *
 * @par Confirmed contents
 * - Primitive 1: code 0xE1000105 (draw-mode/tile words) with link tag.
 * - Primitive 2: sprite (code 0x64) at x/y = 0/0, size 0xA0 x 0xF0, colour
 *   0x808080 (neutral), UV 0/0, link tag.
 * - Primitive 3: code 0xE1000115, same link tag.
 * - Primitive 4: sprite at x = 0xA0, y = 0, size 0xA0 x 0xF0, same colour and
 *   UV, link tag.
 *
 * @par Likely role
 * The pair of sprites tiles the 320x240 image area from a 320x240 buffer in
 * two 160-pixel halves (the VRAM split of a 320-wide frame), and the two
 * 0xE1 words select the corresponding texture pages. func_80025370() calls
 * this right after uploading a loading-screen image.
 *
 * @par Matching note
 * The $4/$7 pins, the repeated "if the pool is full, reuse the last slot"
 * blocks and the @c goto labels are the retail control flow and register
 * allocation; do not tidy them.
 */
void func_8002549C(int value) {
    u8 *packet;
    register int arg __asm__("$7") = value;

    if (gPacketCursor < gPacketLimit) goto p1_alloc;
    __asm__("");
    packet = gPacketCursor; goto p1;
p1_alloc:
    packet = gPacketCursor; gPacketCursor = packet + 28;
p1:
    {
        register u32 mask __asm__("$4") = 0x00FFFFFF;
        ((Page *)packet)->hdr.word = (((Page *)packet)->hdr.word & 0xFF000000) | (arg & mask);
        ((Page *)packet)->hdr.bytes[3] = 1;
        ((Page *)packet)->code = 0xe1000105;
    }

    if (gPacketCursor < gPacketLimit) goto s1_alloc;
    __asm__("");
    packet = gPacketCursor; goto s1;
s1_alloc:
    packet = gPacketCursor; gPacketCursor = packet + 28;
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

    if (gPacketCursor < gPacketLimit) goto p2_alloc;
    __asm__("");
    packet = gPacketCursor; goto p2;
p2_alloc:
    packet = gPacketCursor; gPacketCursor = packet + 28;
p2:
    {
        register u32 mask __asm__("$4") = 0x00FFFFFF;
        ((Page *)packet)->hdr.word = (((Page *)packet)->hdr.word & 0xFF000000) | (arg & mask);
        ((Page *)packet)->hdr.bytes[3] = 1;
        ((Page *)packet)->code = 0xe1000115;
    }

    if (gPacketCursor < gPacketLimit) goto s2_alloc;
    __asm__("");
    packet = gPacketCursor; goto s2;
s2_alloc:
    packet = gPacketCursor; gPacketCursor = packet + 28;
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
