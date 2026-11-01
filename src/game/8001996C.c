#include "game/types.h"

typedef struct Blk { u32 tag; u32 len; u32 body[1]; } Blk;

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
