// Categories: Game Engine > Object Setup > Message 2 | Gameplay > Object Attachment | Graphics > 3D > Actor Transform
#include "game/types.h"

struct Unk2C584Def {
    void *handler;
    void *buf;
    void *state;
    s32 zero;
};

struct Unk2C584Ent {
    u8 unk00[0x24];
    u32 flags;
    u8 unk28[0xC];
    s32 unk34;
};

struct V2 {
    s32 w[2];
};

struct V4 {
    struct V2 a;
    struct V2 b;
};

struct Unk2C584Inner {
    u8 unk00[4];
    struct V4 unk04;
    struct V4 unk14;
    struct V4 unk24;
};

struct Unk2C584Buf {
    s32 unk00;
    struct V4 unk04;
};

struct Unk2C584Data {
    u8 unk00[4];
    struct Unk2C584Inner *unk04;
    s32 *unk08;
};

struct Unk2C584State {
    u8 unk00[4];
    u8 unk04;
    u8 unk05[0x1B];
    u8 unk20;
    u8 unk21;
    u8 unk22[2];
    u8 unk24[0x10];
    u8 unk34;
    u8 unk35;
    u16 unk36;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u16 unk3E;
    u8 unk40;
    u8 unk41;
    u16 unk42;
    u8 unk44[4];
    u8 unk48[6];
    u16 unk4E;
};

struct Unk2C584Obj {
    u8 unk00[4];
    u8 unk04[0x20];
    u32 flags;
    struct Unk2C584Ent *unk28;
    u8 unk2C[0x24];
    u8 unk50[4];
    s32 unk54;
    u8 unk58[4];
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72[0x2E];
    u8 unkA0[0x28];
    u8 unkC8[0x14];
    u8 unkDC_pad[0];
    u16 unkDC;
    u8 unkDE;
    u8 unkDF;
    u32 unkE0;
    u8 unkE4[4];
    s16 unkE8;
    u16 unkEA;
    u16 unkEC;
    u8 unkEE[2];
    u16 unkF0;
    u16 unkF2;
    u16 unkF4;
    u8 unkF6;
    u8 unkF7;
    u8 unkF8;
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    u16 unkFC;
    u8 unkFE;
    u8 unkFF;
    struct Unk2C584Obj *unk100;
    struct Unk2C584Obj *unk104;
    s32 unk108[4];
};

extern u8 gType11EntityHandler[] __asm__("D_8002DEA8");

extern s32 gEntityBaseHandler(s32 message, struct Unk2C584Obj *obj, void *data, s32 arg3) __asm__("func_8002AD58");
extern struct Unk2C584Obj *gSpawnQueuedEntity(void *def) __asm__("func_8004D444");
extern void gTransformPartVertex(void *a, s32 b, void *c) __asm__("func_80017668");
extern void gGteLoadMatrix(void *a) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");
extern void gBuildRotationMatrix(s32 a, s32 b, s32 c, void *d) __asm__("func_800188D0");
extern void gInitAttackSchedulerSlot(void *a, void *b) __asm__("func_80060668");
extern s32 gQuerySurfaceHeight(void *a, s32 b) __asm__("func_80067684");
extern void gSetAnimClipTable(void *a, void *b, s32 c, s32 d) __asm__("func_80044988");
extern s32 func_8006314C(void *a, s32 b);
void gInitObjectFromDef(struct Unk2C584Obj *obj, struct Unk2C584Data *data, s32 arg2) __asm__("func_8002C584");


/**
 * @brief Message-2 handler of an object class: rebuilds one object from its
 *        definition block, spawns the attached child entities and derives every
 *        movement and animation field the tick handler later consumes.
 *
 * Confirmed role
 * --------------
 * func_8002D664 is the message table of this class; case 2 - "initialise or
 * refresh" - calls this function and returns 0. The twin tick handler
 * func_8002CC30 (message 6, same file family) reads exactly the fields written
 * here, so this is the set-up half of the pair.
 *
 * @param obj  Object block (0x118 bytes) that is filled in place.
 * @param data Definition block: +0x0 unused here, +0x4 an inner record, +0x8
 *        the state block (0x50 bytes) that carries all parameters.
 * @param arg2 Forwarded unchanged to func_8002AD58(2, ...) - the default
 *        handler for message 2.
 *
 * @return Nothing.
 *
 * @par Confirmed first steps
 * st = data+0x8 and the scratch value at st+0x20 is saved. Bit 0x80 of the flag
 * byte st+0x40 clears st+0x20 for good. func_8002AD58(2, obj, data, arg2) is
 * then called before anything else, so the default handler always runs first.
 *
 * @par Confirmed child entities (kind 8, 0x21, 0x24)
 * The kind is st+0x4. For these three kinds a definition record is built:
 * handler = D_8002DEA8, buffer = a local copy of the V4 at inner+0x14, state =
 * st, zero = 0, and obj+0x104 = gSpawnQueuedEntity(record) creates the child. The
 * child gets a back pointer to obj at +0xB4, both entities receive bit 0x800000
 * (plus 0x200000 on the child) and obj+0x24 gets 0x800000. st+0x21 is saved,
 * cleared and restored around the call, and st+0x20 becomes 0 during the call
 * unless bit 0x40 is set.
 *
 * Kind 0x21 additionally builds a second child, but from the data block itself
 * (gSpawnQueuedEntity(data), not from the local record): st+0x38 is
 * swapped with st+0x42, the inner V4 pair at +0x4 is swapped with the pair at
 * +0x24 and st+0x4 is temporarily set to 6 so the child is created with the
 * "empty" kind; all three are restored afterwards. obj+0x100 = the second child,
 * which is linked back through its +0x100/+0x104 fields, receives bit 0x800000,
 * and gets the position of the first child's model part 1 (func_80017668) plus
 * its own matrix and part transform.
 *
 * Kind 0x24 copies st+0x48[0..5] to obj+0xF6..+0xFB, st+0x4E to obj+0xFC,
 * writes 0 to obj+0xFE and sets bit 0 of obj+0x24.
 *
 * @par Confirmed non-child kinds
 * Both child pointers are cleared. Kind 0x1E fills the four words at obj+0x108
 * from four pointer slots of the inner record (index 2 and the pointers at
 * +0x18, +0x20, +0x28), each dereferenced with a null check.
 *
 * @par Confirmed orientation and motion fields
 * +0xDF = 0x10 for bit 0 of st+0x40 and 8 for bit 1. The heading st+0x38 is
 * turned into the matrix (func_800188D0) and copied to +0xEC and +0xF2, +0xF4
 * is zeroed. Bits 4 or 1 of st+0x40 set +0xE8 = (st+0x3C * 16 / 218) >> 4, +0xEA
 * = st+0x3E, +0xDF bit 1, +0xF0 = 0, +0xE0 = 0, +0xDE = st+0x35 and +0xDC =
 * st+0x36. Bit 8 instead sets +0xE8 = (st+0x3C * 16 / 300) >> 4, +0xF0 = st+0x38
 * and +0xDF bit 2.
 *
 * @par Confirmed position, script and animation
 * With a child the position at +0x50 is read from its model part 0
 * (func_80017668); without one +0x54 = func_80067684(+0x50, -1 / -2 / value - 2)
 * + (st+0x3A << 4), i.e. the ground height plus a lift. func_80060668 copies the
 * 16-byte block st+0x24 to obj+0xA0 (Likely the script/command state). When the
 * model instance at +0x28 reports +0x10 non-zero the animation track at +0xC8 is
 * attached with func_80044988. Bit 0x10 of st+0x40 calls func_8006314C(obj, 0)
 * and sets +0xDF bit 0x20; bit 0x20 sets +0xDF bit 0x40.
 *
 * @par Likely
 * The divisors 218 and 300 on st+0x3C convert two different parameter scales
 * into the same animation tempo, so st+0x35 might select a unit (frames versus
 * something else) - the same field is copied to +0xDE, which the tick handler
 * uses as a "kind". +0x100 means "has a second visual" from the linkage pattern.
 *
 * @par Unknown
 * The semantics of st+0x34 (mapped to -1, -2 and value - 2 for the ground
 * height call), st+0x3E, st+0x42 and st+0x4E are only visible as constants
 * flowing into fields that other functions consume.
 *
 * @par Globals
 * D_8002DEA8 at 0x8002DEA8 - update handler installed into every child record
 * this function builds.
 *
 * @par Object offsets written (Confirmed)
 * +0x24 flags, +0x2C matrix, +0x50 position, +0x54 ground height, +0x5C
 * transform destination, +0xA0 16-byte state copy, +0xC8 animation track, +0xDC
 * kind, +0xDE/+0xDF flags and kind, +0xE0, +0xE8 speed, +0xEA, +0xEC/+0xF2
 * heading, +0xF0 travel limit, +0xF4, +0xF6..+0xFB six bytes, +0xFC, +0xFE,
 * +0x100/+0x104 children, +0x108 four words.
 *
 * @par Control flow
 * 1. Save st+0x20, apply bit 0x80, forward message 2.
 * 2. Kind switch: build children 8/0x21/0x24, or clear them and handle 0x1E.
 * 3. Restore st+0x20, derive +0xDF from the flags.
 * 4. Heading, speed, travel limit and kind fields.
 * 5. Position (child or ground), script block, animation track.
 * 6. Matrix and part transform, two optional flag bits.
 *
 * @par Calls
 * func_8002AD58 (message 2), gSpawnQueuedEntity (create child, two sites),
 * func_80017668 (read model part, two sites), func_80067684 (ground height),
 * func_80060668 (16-byte copy), func_80044988 (attach animation),
 * func_800188D0 (matrix), func_80010750 (matrix to GTE), func_80010234 (part
 * transform), func_8006314C.
 *
 * @par Side effects
 * Allocates up to two entities, writes their flags and back pointers, mutates
 * the state block temporarily (st+0x4, +0x20, +0x21, +0x38, and the inner V4
 * pairs) and restores it; rewrites almost the whole object block and loads the
 * GTE matrix. No global variable is written besides the entity pool behind
 * gSpawnQueuedEntity.
 *
 * @par Callers
 * func_8002D664, message 2 (analysis/edges.json, kind "direct").
 *
 * @par Environment path
 * Message-2 (init) handler for the object classes dispatched through
 * func_8002D664 (class ids 6, 8, 30, 33 and 36 in the func_8002AE54 table):
 * it forwards the message to the generic setup func_8002AD58 first. For state
 * kinds 8, 0x21 and 0x24 it spawns a companion entity with gSpawnQueuedEntity
 * whose handler is func_8002DEA8 (the class-11 handler, referenced as data
 * symbol D_8002DEA8) and links it through +0x104/+0xB4; kind 0x21
 * additionally spawns a kind-6 copy of itself with swapped path points into
 * +0x100, and kind 0x24 copies six bytes plus a word of extra parameters. It
 * then builds the heading matrix with func_800188D0, derives speed fields
 * from the divisors 218 and 300, takes the ground height from func_80067684
 * (or the companion's part position via func_80017668), attaches the
 * animation through func_80044988 and transforms the reference vertex; it is
 * object setup, not draw code.
 */
void gInitObjectFromDef(struct Unk2C584Obj *obj, struct Unk2C584Data *data, s32 arg2)
{
    struct Unk2C584State *st;
    struct Unk2C584Data *d;
    struct Unk2C584Def def;
    struct Unk2C584Buf buf;
    u32 s7;
    u32 s6;
    u32 s3;
    s32 s0;
    struct Unk2C584Inner *p;
    struct Unk2C584Inner *p2;
    s32 *q;
    struct V4 tmp;
    s32 i;
    s32 off;
    s32 *pp;
    s32 **r;

    d = data;
    st = (struct Unk2C584State *)data->unk08;
    s7 = st->unk20;
    if (st->unk40 & 0x80) {
        st->unk20 = 0;
    }
    gEntityBaseHandler(2, obj, data, arg2);
    if (st->unk04 == 8 || st->unk04 == 0x21 || st->unk04 == 0x24) {
        p = d->unk04;
        def.handler = gType11EntityHandler;
        def.buf = &buf;
        def.zero = 0;
        def.state = st;
        s6 = st->unk21;
        {
            u32 t = 0;
            if (!(st->unk40 & 0x40)) {
                t = s7;
            }
            st->unk20 = t;
        }
        st->unk21 = 0;
        buf.unk04.a = p->unk14.a;
        buf.unk04.b = p->unk14.b;
        obj->unk104 = gSpawnQueuedEntity(&def);
        *(struct Unk2C584Obj **)((u8 *)obj->unk104 + 0xB4) = obj;
        obj->unk104->flags |= 0x200000;
        obj->unk104->flags |= 0x800000;
        obj->flags |= 0x800000;
        s3 = st->unk04;
        if (s3 == 0x21) {
            s0 = st->unk38;
            st->unk38 = st->unk42;
            p2 = d->unk04;
            tmp.a = p2->unk04.a;
            tmp.b = p2->unk04.b;
            p2->unk04.a = p2->unk24.a;
            p2->unk04.b = p2->unk24.b;
            st->unk04 = 6;
            obj->unk100 = gSpawnQueuedEntity(d);
            st->unk38 = s0;
            st->unk04 = s3;
            obj->unk100->unk100 = obj;
            obj->unk100->unk104 = obj->unk104;
            obj->unk100->flags |= 0x800000;
            gTransformPartVertex((u8 *)obj->unk104 + 0x28, 1, (u8 *)obj->unk100 + 0x50);
            gGteLoadMatrix((u8 *)obj->unk100 + 0x2C);
            gGteTransformVerticesWide((u8 *)obj->unk100->unk28->unk34 + ((obj->unk100->unk70 << 4) + 8),
                          (u8 *)obj->unk100 + 0x5C, 1);
        } else {
            obj->unk100 = 0;
        }
        st->unk21 = s6;
        if (st->unk04 == 0x24) {
            obj->unkF6 = st->unk48[0];
            obj->unkF7 = st->unk48[1];
            obj->unkF8 = st->unk48[2];
            obj->unkF9 = st->unk48[3];
            obj->unkFA = st->unk48[4];
            obj->unkFB = st->unk48[5];
            obj->unkFC = st->unk4E;
            obj->unkFE = 0;
            obj->flags |= 1;
        }
    } else {
        obj->unk104 = 0;
        obj->unk100 = 0;
        if (st->unk04 == 0x1E) {
            r = (s32 **)data->unk04;
            obj->unk108[0] = r[2] ? *r[2] : 0;
            i = 1;
            off = 0;
            for (; i < 4; i++) {
                s32 val = 0;
                s32 *e = *(s32 **)((u8 *)r + off + 0x18);
                if (e) {
                    val = *e;
                }
                obj->unk108[i] = val;
                off += 8;
            }
        }
    }
    st->unk20 = s7;
    obj->unkDF = 0;
    if (st->unk40 & 1) {
        obj->unkDF = 0x10;
    }
    if (st->unk40 & 2) {
        obj->unkDF |= 8;
    }
    gBuildRotationMatrix(st->unk38, 0, 0, (u8 *)obj + 0x2C);
    {
        u16 h = st->unk38;
        obj->unkF4 = 0;
        obj->unkEC = h;
        obj->unkF2 = h;
    }
    if ((st->unk40 & 4) || (st->unk40 & 1)) {
        obj->unkE8 = (st->unk3C * 16 / 218) >> 4;
        obj->unkEA = st->unk3E;
        obj->unkDF |= 1;
        obj->unkF0 = 0;
        obj->unkE0 = 0;
        obj->unkDE = st->unk35;
        obj->unkDC = st->unk36;
    }
    if (st->unk40 & 8) {
        obj->unkE8 = (st->unk3C * 16 / 300) >> 4;
        obj->unkF0 = st->unk38;
        obj->unkDF |= 2;
    }
    if (obj->unk104) {
        gTransformPartVertex((u8 *)obj->unk104 + 0x28, 0, (u8 *)obj + 0x50);
    } else {
        s32 a;
        s32 v = (s8)st->unk34;
        a = v == 0 ? -1 : v == 1 ? -2 : v - 2;
        obj->unk54 = gQuerySurfaceHeight((u8 *)obj + 0x50, a) + (st->unk3A << 4);
    }
    gInitAttackSchedulerSlot(st->unk24, (u8 *)obj + 0xA0);
    if (*(s32 *)((u8 *)obj->unk28 + 0x10)) {
        gSetAnimClipTable(*(void **)((u8 *)obj->unk28 + 0x10), (u8 *)obj + 0xC8, 1, 0);
    }
    gGteLoadMatrix((u8 *)obj + 0x2C);
    gGteTransformVerticesWide((u8 *)obj->unk28->unk34 + ((obj->unk70 << 4) + 8), (u8 *)obj + 0x5C, 1);
    if (st->unk40 & 0x10) {
        func_8006314C(obj, 0);
        obj->unkDF |= 0x20;
    }
    if (st->unk40 & 0x20) {
        obj->unkDF |= 0x40;
    }
}
