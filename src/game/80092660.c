// Categories: Graphics > Display Lists | Graphics > Primitives
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
struct Rect { s16 x, y, w, h; };

struct Packet { u8 pad[3]; u8 len; u32 cmd, mode, xy, pos, wh; };
void gSetDrawMovePacket(struct Packet *p, struct Rect *r, int x, int y) __asm__("func_80092660");
/**
 * @brief Builds a five-word display-list entry describing one rectangle.
 *
 * @param p Target packet.
 * @param r Source rectangle (x, y, w, h as half-words).
 * @param x Destination x (11 bits), y Destination y (16 bits).
 *
 * @par Confirmed field values
 * cmd = 0x01000000, mode = 0x80000000, xy = the rectangle's x/y pair,
 * wh = its w/h pair, pos = (y << 16) | (x & 0xFFFF), and length = 5 - or 0 when
 * the rectangle has no area.
 *
 * @par Likely meaning
 * 0x01000000 is also the value func_800926C0 writes as the terminator behind a
 * VRAM transfer, so this is the engine's own list format rather than a raw GPU
 * command: a rectangle plus a destination offset, closed by a fixed tag.
 *
 * @par Callers
 * func_800259D8, to draw the screen-filling backdrop quad.
 *
 * @par Environment path
 * Builds a VRAM copy packet: length 5 (0 when the rectangle has no area), GP0
 * 01 to flush the texture cache, GP0 80 copy command, the rectangle xy as
 * source, (y shifted left 16) or (x and 0xFFFF) as destination and the
 * rectangle wh. func_800259D8 uses it with a 320x240 rectangle at y 0x100
 * minus the screen offset and destination 0, gScreenOffsetValue to copy the
 * frame between the two buffers.
 */
void gSetDrawMovePacket(struct Packet *p, struct Rect *r, int x, int y)
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
