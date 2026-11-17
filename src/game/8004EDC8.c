// Categories: Graphics > Renderer > Object Pass | Graphics > Primitives
#include "game/types.h"

struct Node { u8 pad0[8]; s32 count; u8 padc[0x17]; u8 type; u32 flags; u8 pad28[0x28]; s32 pos[3]; };
struct Extra { u8 pad0[0xa4]; u32 flags; u8 types[8]; s16 coords[16]; };
struct Ctl { u8 pad0[0xc1]; s8 mode; u8 padc2[0x23e]; struct Extra *extra; };
struct Obj { u8 pad0[0xac]; struct Ctl *ctl; };
struct Packet { u32 tag; u32 cmd; };
extern struct Packet *gPacketCursor __asm__("D_8009FA90");
extern struct Packet *gPacketLimit __asm__("D_8009F9C0");
extern s32 D_800A8F90[];
extern s32 D_8009F7B0;
extern struct Node *D_800A8F98[];
extern u32 D_8009FAA4;
extern void *D_800A6F60[];
extern void func_8005A9C4(struct Obj *);
extern void func_8005B088(s32 *, void *, s32, s32);

/**
 * @brief Draws one object's sub-parts: a texpage word plus one primitive per
 *        listed node.
 *
 * @param obj Object: the controller block at +0xAC, whose mode (0 = small,
 *            otherwise large) selects a carrier size of 0xC or 0x5C bytes.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. func_8005A9C4(obj) prepares the object.
 * 2. One packet with the fixed command 0xE100069E (texpage/draw mode) is emitted.
 * 3. The nodes of D_800A8F98 (count in D_800A8F90) are drawn with
 *    func_8005B088(node->pos, D_800A6F60[type & 31], size, type >> 5) when the
 *    node has a count, a type, none of the flag bits 0x30, and either a type
 *    below 23 or the "show all" bit 0x80 of D_8009FAA4.
 * 4. A second loop walks the eight optional extra slots of the controller
 *    (flags bitmask 0x924924): bit 2 per entry marks a used slot, its type and
 *    two 9-bit-scaled coordinates are taken from the extra block.
 *
 * @par Likely role
 * The multi-part object renderer (vehicles and turrets made of several nodes).
 */
void func_8004EDC8(struct Obj *obj)
{
    struct Node **p;
    struct Node *n;
    struct Extra *extra;
    struct Packet *packet;
    s32 count, size, enabled;
    u32 flags;
    s32 type;
    s32 pos[3];
    func_8005A9C4(obj);
    size = 0x5c;
    if (obj->ctl->mode == 0) size = 0xc;
    p = D_800A8F98;
    packet = gPacketCursor;
    if (packet < gPacketLimit) gPacketCursor = (struct Packet *)((u8 *)packet + 0x1c);
    packet->cmd = 0xe100069e;
    packet->tag &= 0xff000000;
    ((u8 *)packet)[3] = 1;
    enabled = D_8009FAA4 & 0x80;
    count = D_800A8F90[0];
    while (count > 0) {
        n = *p++;
        if (n->count > 0 && (type = n->type) != 0 && !(n->flags & 0x30)) {
            if ((type & 31) < 23 || enabled)
                func_8005B088(n->pos, D_800A6F60[type & 31], size, type >> 5);
        }
        count--;
    }
    extra = obj->ctl->extra;
    if (extra && ((flags = extra->flags) & 0x924924)) {
        for (count = 0; count < 8; flags >>= 3, count++) {
            pos[1] = 0;
            if (flags & 4) {
                type = extra->types[count];
                pos[0] = extra->coords[count * 2 + 1] << 9;
                pos[2] = extra->coords[count * 2] << 9;
                if ((type & 31) < 23 || enabled)
                    func_8005B088(pos, D_800A6F60[type & 31], size, type >> 5);
            }
        }
    }
}
