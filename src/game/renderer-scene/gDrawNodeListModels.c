// Categories: Graphics > Renderer > Depth Cue Pass | Graphics > Culling
#include "game/types.h"

struct Obj8003CF6C {
    u8 unk00[0x28];
    s32 x;
    s32 y;
    s32 z;
    u8 unk34[0xC];
    s32 unk40;
    u8 unk44[6];
    u8 flags4A;
};

struct Node8003CF6C {
    struct Node8003CF6C *next;
    u8 unk04[0x30];
    struct Obj8003CF6C *obj;
    u8 unk38;
    u8 flags39;
    u8 unk3A[6];
    u16 unk40;
    u8 unk42;
};

extern struct Node8003CF6C *gAnchorNodeList __asm__("D_8009F320");

extern s32 gVectorLength3D(s32 *vector) __asm__("func_8001A100");
extern void gSetModelUvOffset(s32 low, s32 high) __asm__("func_80013AA0");
extern u8 gSubmitModelInstance(struct Obj8003CF6C *instance, u32 *flags, s32 arg2) __asm__("func_80017508");
extern void gDrawModelFrame(struct Obj8003CF6C *obj) __asm__("func_800164F4");
void gDrawNodeListModels(s32 arg0) __asm__("func_8003CF6C");


/**
 * @brief Walks the depth-cued object list and draws the visible nodes.
 *
 * @param arg Frame argument forwarded to the visibility test.
 * @return Nothing.
 *
 * @par Confirmed
 * Iterates D_8009F320 with `next` captured first. A node is skipped when its
 * object pointer is null or bit 1 of the flags byte at +0x39 is clear, and when
 * `func_8001A100(vec) - obj->unk40 > 0x1FFFF` (the radius test against the
 * object's distance word). Surviving nodes hand their two bytes at +0x40/+0x41 to
 * func_80013AA0 (the GTE depth-cue pair), func_80017508(obj, &flags, +0x42)
 * decides visibility, and func_800164F4(obj) draws the frame.
 *
 * @par Likely role
 * The "far objects, depth-cued" pass, sibling of func_80075A84 and driven from
 * the same render list.
 *
 * @par Environment path
 * One of the scene passes of func_8003F6E4: it walks the node list
 * D_8009F320, skips nodes without an object or without bit 1 of flag byte
 * +0x39, and drops objects whose distance from the passed view position minus
 * their radius exceeds 0x1FFFF (func_8001A100). Each survivor is submitted
 * with gSubmitModelInstance (func_80017508); when the node's +0x40 half-word
 * is non-zero, func_80013AA0 installs the node's UV offset bytes before the
 * submit and resets it to (0, 0) afterwards, and objects with flag 0x10 at
 * +0x4A are additionally drawn with gDrawModelFrame (func_800164F4).
 */
void gDrawNodeListModels(s32 arg0)
{
    struct Node8003CF6C *node;
    s32 vec[3];
    u32 flags;
    s32 *origin;

    origin = (s32 *)arg0;
    flags = 0;
    for (node = gAnchorNodeList; node != 0; node = node->next) {
        if (node->obj != 0 && (node->flags39 & 2)) {
            vec[0] = node->obj->x - origin[0];
            vec[1] = node->obj->y - origin[1];
            vec[2] = node->obj->z - origin[2];
            if (gVectorLength3D(vec) - node->obj->unk40 > 0x1FFFF) {
                continue;
            }
            if (node->unk40 != 0) {
                gSetModelUvOffset(((u8 *)node)[0x40], ((u8 *)node)[0x41]);
            }
            gSubmitModelInstance(node->obj, &flags, node->unk42);
            if (node->unk40 != 0) {
                gSetModelUvOffset(0, 0);
            }
            if (node->obj->flags4A & 0x10) {
                gDrawModelFrame(node->obj);
            }
        }
    }
}
