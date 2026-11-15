// Categories: Resources > Asset Loading | Graphics > Textures
#include "game/types.h"

typedef struct Blk { u32 tag; u32 len; u32 body[1]; } Blk;

/**
 * @brief Parses a chunked texture container and wires the pixel/CLUT pointers
 *        into its CCB header.
 *
 * @param data Container start.
 * @param size Container byte size.
 * @return The CCB body pointer (header + 4), with [10] = pixel data and
 *         [11] = CLUT set; 0-derived when no CCB chunk exists.
 *
 * @par Confirmed
 * Walks {u32 tag, u32 len, body[]} blocks: tag 'CCB ' (0x43434220) sets
 * ccb = body + 1, 'PLUT' (0x504C5554) sets plut = body + 1, 'PDAT'
 * (0x50444154) and 'PIX4' (0x50495834) set pdat = body. After the walk,
 * ccb[10] = pdat and ccb[11] = plut.
 *
 * @par Likely role
 * The texture asset unpacker of the upload path func_80063C5C: the four
 * tags are the game's container format for colour-conversion blocks,
 * palette LUTs and 4/16-bit pixel data.
 *
 * @par Unknown
 * The exact CCB layout beyond words 10/11 and whether PIX4 vs PDAT selects
 * the bit depth.
 *
 * @par Callers
 * func_80063C5C.
 */
u32 *func_8001996C(u8 *data, s32 size)
{
    u32 *ccb = 0;
    u32 *pdat = ccb;
    u32 *plut = ccb;
    Blk *next = (Blk *)data;
    Blk *cur;
    u32 *body;

    do {
        cur = next;
        body = cur->body;
        switch (cur->tag) {
        case 0x43434220:
            ccb = body + 1;
            break;
        case 0x504C5554:
            plut = body + 1;
            break;
        case 0x50444154:
        case 0x50495834:
            pdat = body;
            break;
        }
        next = (Blk *)((u8 *)next + cur->len);
    } while ((u8 *)next - data < size);

    ccb[10] = (u32)pdat;
    ccb[11] = (u32)plut;
    return ccb;
}
