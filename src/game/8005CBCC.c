// Categories: Graphics > Renderer > Map Cell Sprite | Graphics > Effects > Object-Attached Effect | Gameplay > Object Attachment
#include "game/types.h"

extern int gWeaponFireDispatch();

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

struct Unk5CB80Node {
    struct Unk5CB80Node *next;
    void *first;
    void *second;
    u8 unk0C[0x1C];
    u8 flags;
};

struct Unk5CB80Pool {
    void *storage;
    struct Unk5CB80Node *active;
    struct Unk5CB80Node *free;
};

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

struct Unk5CBCCNode {
    struct Unk5CBCCNode *next;
    void *first;
    void *second;
    u8 unk0C[4];
    void *unk10;
    void *unk14;
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F;
    struct Unk5CBCCNode *self;
    void (*callback)(void);
    u8 flags;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    Vec3 vec;
};

struct Unk5CBCCLocal {
    u8 unk00[0x10];
    Vec3 vec;
    s32 out[9];
    u8 unk40[9];
    u8 flag;
    u8 unk4A[6];
    Vec3 temp;
};

struct Unk5CBCCArg0 {
    u8 unk00[8];
    void *unk08;
};

struct Unk5CBCCArg2 {
    u8 unk00[8];
    void *unk08;
    u8 unk0C[0x50];
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 unk68[0x10];
    u8 unk78[0xC];
    s32 unk84;
};

struct Unk5CBCCArg5 {
    u16 unk00;
    u8 unk02[4];
    s16 unk06;
    u8 unk08[2];
    s16 unk0A;
    u8 unk0C[2];
    u8 unk0E;
    u8 unk0F;
    u8 unk10[3];
    u8 unk13;
};

struct ModelInstance {
    void *model;
    u8 coord[0x44];
    s16 frame;
};

struct Unk402DCOwner {
    u8 unk00[8];
    s32 unk08;
};

extern struct Unk5CB80Pool gOwnerNodeListB __asm__("D_800A22B0");
extern u8 gSpawnTypeFlags[] __asm__("D_800A6578");

extern struct Unk5CB80Node *func_8005CB80(struct Unk5CB80Pool *pool, void *target);
extern struct ListNode *gTakePoolNode(struct ListNode *node) __asm__("func_8003BEF0");
extern void gTransformPartVertex(struct ModelInstance *instance, s32 part, s32 *out) __asm__("func_80017668");
extern void gSolveLeadDirection(s32 *a, s32 *b, s32 c, s32 d, void *e, void *f, s32 g) __asm__("func_8005E2A0");
extern void gNormalizeVector(s32 *vec) __asm__("func_8001A180");
extern void gBuildBasisFromAxis(s32 *a, s32 *b) __asm__("func_800193F8");
extern s32 func_8003BF50(struct Unk5CBCCArg0 *arg0);
extern s32 gSpawnTypeAtOwner(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");
extern void gAttachedPartEffectHandler(void) __asm__("D_8005CA04");

/**
 * @brief Attaches a modelled sprite effect to one part of an object and submits it
 *        to the draw list of a map cell.
 *
 * @param arg0   Owner object; its field at +0x08 is copied into the node at +0x14.
 *               Also the key looked up in the effect pool.
 * @param arg1   Not referenced by the body.
 * @param arg2   Optional configuration: target point +0x5C/+0x60/+0x64, pointers
 *               +0x78 and the value +0x84 used by the direction helper.
 * @param arg3   Model instance: part positions come from func_80017668, the
 *               coordinate matrix from +0x04 (nine words).
 * @param partin Model part index; sign extended, and a negative value aborts the
 *               creation path.
 * @param arg5   Effect definition: speed at +0x06, spread at +0x0A, column at +0x0F,
 *               id at +0x00, byte at +0x0E and byte at +0x13.
 *
 * @return Nothing.
 *
 * @par Confirmed control flow
 * 1. func_8005CB80(&D_800A22B0, arg0) returns an existing node for the owner; when
 *    found, only the lifetime (+0x18) and the payload at +0x04/+0x10 are refreshed.
 * 2. Otherwise a node is taken with func_8003BEF0; a null instance or a negative part
 *    index returns first. func_80017668(instance, part, &pos) yields the part
 *    position in world space, which is copied into the node's vector field.
 * 3. With a configuration the orientation is either the randomised vector from
 *    gSolveLeadDirection(pos, temp, speed << 4, spread << 4, arg2+0x5C, arg2+0x78,
 *    arg2+0x84) when the speed is positive, or the plain vector from arg2+0x5C to
 *    the part position normalised by func_8001A180; func_800193F8 then builds the
 *    nine-word matrix. Without a configuration the matrix is copied verbatim from
 *    the instance coordinate block.
 * 4. A local block is prepared with its flag byte set to 0x0C, so the pointer handed
 *    to func_800402DC is never the null alternative. func_8003BF50(arg0) is called
 *    twice for the same row; the byte at D_800A6578[(column << 2) + (row << 10)] has
 *    bit 2 cleared in the first pass and bit 0 set in the second.
 * 5. node->self and node->callback (D_8005CA04) are installed and
 *    func_800402DC(0, column, &local, row, &node->self) submits the draw entry. A
 *    return of 0 sets node->flags = 1 and leaves the remaining fields unset.
 * 6. On success the node receives +0x2B = arg5+0x13, +0x1C = arg5+0x00, +0x08 =
 *    arg0, +0x1E = arg5+0x0E, flags 0, +0x2A = 0, +0x14 = arg0->+0x08 and +0x29 = 0.
 * 7. Common tail: +0x18 = 0x96 (a 150 frame lifetime), +0x04 = arg2 and, when
 *    arg2 is present, +0x10 = arg2->+0x08.
 *
 * @par Likely role
 * The spawn of a short-lived drawn quad or sprite anchored to a model part -
 * muzzle flash, exhaust plume, damage spark - inserted into the polygon list of the
 * map cell identified by the row/column pair, with a per-cell visibility byte in
 * D_800A6578.
 *
 * @par Unknown
 * - Whether D_800A6578 is one byte per map cell or one byte per column/row slot; the
 *   index (column << 2) + (row << 10) fits a 0x100-wide row grid.
 * - The exact callback contract of D_8005CA04, installed but called elsewhere.
 *
 * @par Callers
 * func_8005F6E8 (message 17) and gWeaponFireDispatch (message 0x11).
 */
void gAttachPartEffect(struct Unk5CBCCArg0 *arg0, s32 arg1, struct Unk5CBCCArg2 *arg2,
                   struct ModelInstance *arg3, s32 partin, struct Unk5CBCCArg5 *arg5) __asm__("func_8005CBCC");

void gAttachPartEffect(struct Unk5CBCCArg0 *arg0, s32 arg1, struct Unk5CBCCArg2 *arg2,
                   struct ModelInstance *arg3, s32 partin, struct Unk5CBCCArg5 *arg5)
{
    struct Unk5CBCCLocal local;
    struct Unk5CBCCNode *node;
    s32 row;
    u8 *p;
    u8 *base;
    register s32 tmp __asm__("$2");
    register struct Unk5CBCCArg0 *owner __asm__("$3");
    struct Unk5CB80Pool *pool = &gOwnerNodeListB;
    register s32 part __asm__("$18") = partin;

    node = (struct Unk5CBCCNode *)func_8005CB80(pool, arg0);
    if (node == 0) {
        local.flag = 0xC;
        if (arg3 == 0) {
            return;
        }

        tmp = part << 24;
        part = tmp >> 24;
        if (part < 0) {
            return;
        }

        node = (struct Unk5CBCCNode *)gTakePoolNode((struct ListNode *)pool);
        if (node == 0) {
            return;
        }

        gTransformPartVertex(arg3, part, &local.vec.x);
        node->vec = local.vec;

        if (arg2 != 0) {
            if (arg5->unk06 > 0) {
                gSolveLeadDirection(&local.vec.x, &local.temp.x, arg5->unk06 << 4,
                              arg5->unk0A << 4, &arg2->unk5C, arg2->unk78, arg2->unk84);
            } else {
                local.temp.x = arg2->unk5C - local.vec.x;
                local.temp.y = arg2->unk60 - local.vec.y;
                local.temp.z = arg2->unk64 - local.vec.z;
                gNormalizeVector(&local.temp.x);
            }
            gBuildBasisFromAxis(&local.temp.x, local.out);
        } else {
            s32 first;
            first = ((s32 *)arg3->coord)[0];
            local.out[1] = ((s32 *)arg3->coord)[1];
            local.out[2] = ((s32 *)arg3->coord)[2];
            local.out[3] = ((s32 *)arg3->coord)[3];
            local.out[4] = ((s32 *)arg3->coord)[4];
            local.out[5] = ((s32 *)arg3->coord)[5];
            local.out[6] = ((s32 *)arg3->coord)[6];
            local.out[7] = ((s32 *)arg3->coord)[7];
            local.out[8] = ((s32 *)arg3->coord)[8];
            local.out[0] = first;
        }

        row = func_8003BF50(arg0);
        base = gSpawnTypeFlags;
        p = &base[(arg5->unk0F << 2) + (row << 10)];
        *p &= 0xFB;

        row = func_8003BF50(arg0);
        p = &base[(arg5->unk0F << 2) + (row << 10)];
        *p |= 1;

        node->self = node;
        node->callback = gAttachedPartEffectHandler;

        if (gSpawnTypeAtOwner(0, arg5->unk0F, local.flag ? (s32)&local : 0, row,
                          (s32)&node->self) == 0) {
            node->flags = 1;
            return;
        }

        __asm__("" : "=r"(owner) : "0"(arg0));
        node->unk2B = arg5->unk13;
        node->unk1C = arg5->unk00;
        node->second = arg0;
        node->unk1E = arg5->unk0E;
        node->flags = 0;
        node->unk2A = 0;
        if (owner != 0) {
            node->unk14 = owner->unk08;
        }
        node->unk29 = 0;
    }

    node->unk18 = 0x96;
    node->first = arg2;
    if (arg2 != 0) {
        node->unk10 = arg2->unk08;
    }
}
