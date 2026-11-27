// Categories: Graphics > GPU Command Lists > VRAM Transfer | Platform > GPU
typedef unsigned int u32;
typedef unsigned char u8;
typedef signed short s16;

struct Rect { s16 x, y, w, h; };
struct Packet { u8 pad[3]; u8 len; u32 cmd, xy, wh; };
void gBuildVramTransferPkt(struct Packet *p, struct Rect *r) __asm__("func_800926C0");


/**
 * @brief Builds a CPU-to-VRAM transfer packet (GP0 0xA0) with its terminator.
 *
 * @param p Target packet.
 * @param r Source rectangle (x, y, w, h as half-words).
 *
 * @par Confirmed
 * size = (w * h + 1) / 2 words, length = size + 4 (three command words plus
 * the payload) - or 0 when size >= 13, i.e. the packet only supports up to 12
 * payload words. cmd = 0xA0000000 (GP0 "CPU to VRAM copy"), xy and wh carry the
 * rectangle, and the word at p + length * 4 is set to 0x01000000, the list
 * terminator that func_80092660 uses as its own command tag.
 *
 * @par Likely role
 * The small upload helper of the 2D layer: textures and palettes are copied in
 * chunks of at most 12 words.
 *
 * @par Callers
 * None in the symbol table (inlined where a small upload is needed).
 *
 * @par Environment path
 * Builds a CPU-to-VRAM transfer packet (GP0 0xA0) with its rectangle
 * parameters and terminator word into packet p (not GP0 0x80 MoveImage).
 */
void gBuildVramTransferPkt(struct Packet *p, struct Rect *r)
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
