// Categories: Graphics > Sprites > Target Markers | Gameplay > Targeting | Graphics > 2D Drawing | Graphics > Clipping
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
extern Camera *gCamera __asm__("D_8009F9E4");
extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");
extern u32 D_800A8EA0[];
extern void gTransformPartVertex(void *, int, Vec *) __asm__("func_80017668");
extern int func_800684AC(Vec *, Vec *, Vec *, int, int);
extern void gGteLoadRotation(void *) __asm__("func_80010820");
extern void gProjectVertexBatch(SVec *, SVec *, int *, int) __asm__("func_8001047C");
/**
 * @brief Draws the marker of an object's current target: a 16x16 sprite at the
 *        target's screen position plus a two-point line from the object to a
 *        computed point.
 *
 * @param obj Object block. Fields used: flags at +0x24, state at +0xAC, model
 *            instance at +0xDC. The target is state + 0x6C and the target
 *            position is at +0x5C..0x64 of that block.
 *
 * @return Nothing.
 *
 * @par Globals
 * - D_8009F9E4 (0x8009F9E4): camera block; the eye position is read at +0x7C,
 *   +0x80, +0x84 and the view matrix at +0x28.
 * - D_8009FA90 (0x8009FA90): packet cursor.
 * - D_8009F9C0 (0x8009F9C0): packet limit.
 * - D_800A8EA0[0] (0x800A8EA0): one 32-bit resource handle; its low 16 bits are
 *   masked with 0x9FF into the tpage word and its high half becomes the sprite
 *   CLUT.
 *
 * @par Control flow
 * Confirmed:
 * 1. Early out when state->target == 0 or when obj->flags & 0x1000000 is set.
 * 2. points[2] = target position. func_80017668(obj->model + 0x28, 0,
 *    &points[0]) fetches the object's own anchor; func_800684AC(&points[0],
 *    &points[2], &points[1], 0, 1) (five arguments, the fifth pushed on the
 *    stack) computes the third point, and points[1] is replaced by the target
 *    position when that call returns 0.
 * 3. Three iterations project every point to 16-bit screen space:
 *    (p - camera eye) >> 4, x at +0x0, y at +0x2, z at +0x4 of an 8-byte entry.
 * 4. func_80010820(camera + 0x28) loads the view matrix and
 *    func_8001047C(screen, screen, &flag, 3) projects and flags the three
 *    points; the draw is skipped when ((unsigned char *)&flag)[2] & 1 is set
 *    (lbu at 0x5A of the frame, i.e. flag + 2).
 * 5. Packet 1 (0x1C bytes): the existing word 0 is masked with 0xFF000000 and
 *    byte 3 is set to 1, so the tag announces one following word; +0x4 =
 *    0xE1000000 | (D_800A8EA0[0] & 0x9FF).
 * 6. Packet 2 (0x1C bytes): word 0 keeps only its top byte (the address part is
 *    cleared), byte 3 = 4, the opcode byte at +0x7 = 0x66, r at +0x4, g at +0x5
 *    and b at +0x6 = 0x80 each, +0xC (u) = 0, +0xD (v) = 0xD2,
 *    +0xE (clut) = handle >> 16,
 *    +0x10 (w) = 0x10, +0x12 (h) = 0x10, +0x8 (x) = screen[2].x - 8,
 *    +0xA (y) = screen[2].y - 8 - a 16x16 quad centred on the target.
 * 7. depth = screen[0].z clamped up to 0x400: the code computes z - 0x400,
 *    takes the negative part with sra 31 and and, and subtracts it from z, so
 *    z < 0x400 becomes exactly 0x400.
 * 8. Packet 3 (0x1C bytes): byte 3 = 1, +0x4 = 0xE10006A0, low 24 bits of
 *    word 0 = depth.
 * 9. Packet 4 (0x1C bytes): byte 3 = 3, +0x7 = 0x42, +0x4 = 0x30 (r), +0x5 =
 *    +0x6 = 0, low 24 bits of word 0 = depth, +0x8/+0xA = screen[0].x/y,
 *    +0xC/+0xE = screen[1].x/y.
 *
 * @par Calls
 * func_80017668, func_800684AC, func_80010820, func_8001047C.
 *
 * @par Side effects
 * Appends up to four 0x1C-byte packets. As in the other packet builders of this
 * file, a packet is only counted when the cursor is below the limit, but the
 * body is written in either case, so the slot at the limit is overwritten
 * rather than skipped.
 *
 * @par Likely role
 * A lock-on / off-screen indicator: the 16x16 textured quad with a fixed v of
 * 0xD2 and the dark red two-point line describe a reticle plus a leader line.
 * The gating condition D_8009FA00 == 4 in the caller supports "only while the
 * game is in one particular mode". The geometry is confirmed; the name of the
 * effect is not.
 *
 * @par Unknown
 * - What func_800684AC computes (only its failure fallback is observable).
 * - What D_800A8EA0[0] points to beyond the tpage and CLUT fields.
 * - Why packet 3 repeats a draw mode with tpage 0x6A0 before the line; the code
 *   stores depth in its tag, which the GPU does not read, so it looks like a
 *   padding/ordering artefact of the original code.
 *
 * @par Callers
 * func_80047F30, recorded at 0x80047F30 (direct call at 0x80047F60), reached
 * only when D_8009FA00 == 4.
 */
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
    gTransformPartVertex(obj->model + 0x28, 0, &points[0]);
    if (!func_800684AC(&points[0], &points[2], &points[1], 0, 1)) points[1] = points[2];
    for (i = 0; i < 3; i++) {
        screen[i].x = (points[i].x - gCamera->pos.x) >> 4;
        screen[i].y = (points[i].y - gCamera->pos.y) >> 4;
        screen[i].z = (points[i].z - gCamera->pos.z) >> 4;
    }
    gGteLoadRotation((u8 *)gCamera + 0x28);
    gProjectVertexBatch(screen, screen, &flag, 3);
    if (((u8 *)&flag)[2] & 1) return;
    if (gPacketCursor < gPacketLimit) goto a1;
    __asm__(""); packet = gPacketCursor; goto p1;
a1: packet = gPacketCursor; gPacketCursor = packet + 28;
p1:
    { register u32 mask __asm__("$3") = 0xff000000;
      __asm__("" : "=r"(mask) : "0"(mask));
      *(u32 *)packet &= mask; }
    tex = D_800A8EA0[0];
    ((Page *)packet)->len = 1;
    ((Page *)packet)->code = (tex & 0x9ff) | 0xe1000000;
    if (gPacketCursor < gPacketLimit) goto a2;
    __asm__(""); packet = gPacketCursor; goto p2;
a2: packet = gPacketCursor; gPacketCursor = packet + 28;
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
    p = gPacketCursor;
    if (p < gPacketLimit) gPacketCursor = p + 28;
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
    next = gPacketCursor;
    __asm__("" : "=r"(next) : "0"(next));
    hdr &= mask;
    { register u8 *end __asm__("$2") = gPacketLimit;
    __asm__("" : "=r"(end) : "0"(end));
    *(u32 *)p = hdr | low;
    if (next < end) goto a3; }
    __asm__(""); p = next; goto p3;
a3: p = next; gPacketCursor = p + 28;
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
