// Categories: Gameplay > Entity Update | Game Engine > Entity Lifecycle | Graphics > Scene Graph
#include "game/types.h"

struct Vec3s32 { s32 x; s32 y; s32 z; };
struct EntityLink { u8 pad[8]; s32 unk08; };
struct Entity28 { u8 pad[0x34]; u8 *unk34; };

struct Entity {
    s32 unk00;
    s32 unk04;
    struct Entity *unk08;
    u16 unk0C;
    u8 unk0E[6];
    u16 unk14;
    u8 unk16[2];
    u8 unk18;
    u8 unk19[3];
    s16 unk1C;
    u8 unk1E[4];
    s8 unk22;
    u8 unk23[5];
    struct Entity28 *unk28;
    u8 unk2C[0x24];
    struct Vec3s32 unk50;
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72;
    u8 unk73[5];
    struct Vec3s32 unk78;
    s32 unk84;
    u8 unk88[0x18];
    struct Vec3s32 unkA0;
    s32 unkAC;
    u8 unkB0[4];
    u8 unkB4;
    u8 unkB5;
    u8 unkB6[0xA];
    struct EntityLink *unkC0;
    s32 unkC4;
    s32 unkC8;
    u8 unkCC[4];
    s32 unkD0;
};

extern void gVectorAddScaled(void *a, void *b, s32 c, s32 *d) __asm__("func_8001ACA4");
extern s32 gTraceMapRay(void *a, s32 *b, s32 c, s32 d, s32 e) __asm__("func_800684AC");
extern s32 gVectorLength3D(s32 *vector) __asm__("func_8001A100");
extern void gNormalizeVector(struct Vec3s32 *vector) __asm__("func_8001A180");
extern struct Entity *func_8004D3FC(s32 size);
extern s32 func_80019668(void);
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");

extern void func_80036940(struct Entity *self, s32 arg2, s32 arg3);
extern void gBuildBasisFromAxisYZ(struct Vec3s32 *axis, void *basis) __asm__("func_8001937C");
extern void gBuildBasisFromAxis(struct Vec3s32 *axis, void *basis) __asm__("func_800193F8");
extern s32 func_8001A750(struct Vec3s32 *a, void *b);
extern void gRetireEntity(struct Entity *entity) __asm__("func_8004D958");
extern void gGteLoadMatrix(void *p) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");
extern void func_80036AB4(struct Entity *self, s32 arg3);
extern s32 gEntityBaseHandler(s32 message, struct Entity *self, s32 arg2, s32 arg3) __asm__("func_8002AD58");
extern void *jtbl_80011380[];
s32 gHandleProjectileMsg(s32 arg0, struct Entity *arg1, s32 arg2, s32 arg3) __asm__("func_80036E6C");


/**
 * @brief Entity message handler dispatched through the jump table at
 *        jtbl_80011380: spawn, movement with orientation choice, team
 *        comparison and guarded forwarding.
 *
 * @param arg0 Message number (1..11 via the table, else base handler).
 * @param arg1 Entity record.
 * @param arg2 Other entity or spawn data, message-dependent.
 * @param arg3 Forwarded value.
 * @return Message 1: 0xD4; message 4: 0/1; others 0 or the base result.
 *
 * @par Confirmed
 * 1: 0xD4. 2: func_80036940. 3: func_80036BD4 updates the entity; when
 * |unk78.x| + |unk78.y| < 0x10, func_8001937C builds the basis from the
 * direction, otherwise func_800193F8; when func_8001A750(&unk50, &unkC8)
 * > 0x58000, func_8004D958 runs; func_80010750 loads the basis and
 * func_80010234 transforms one frame vertex. 4: returns 1 only when
 * unk14 differs and unkC0 != other. 5: unk1C = -1. 6: func_80036AB4 then
 * the base handler. 7..11: forward to func_8002AD58 only when other->unk08
 * is set and (!(other->unk0C & 1) || other->unk08->unk18 != self->unk18).
 *
 * @par Likely role
 * The 0x58000 distance test despawns or recycles entities that strayed
 * too far; the small-angle basis path avoids the full orientation math
 * for nearly axis-aligned movement.
 *
 * @par Callers
 * No direct callers in the call graph; dispatched indirectly through a
 * handler table.
 *
 * @par Environment path
 * Message handler of a projectile entity class (size 0xD4), reached only
 * through its handler table via the jump table jtbl_80011380. Message 2 runs
 * the spawn helper func_80036940; message 3 advances the projectile with
 * func_80036BD4 (gravity-biased motion, child spawn), re-orients the basis
 * along the motion vector with func_8001937C or func_800193F8, frees the
 * entity through func_8004D958 once it is more than 0x58000 from its start
 * point, and refreshes the frame vertex. Message 4 reports whether another
 * entity is a valid target (different team word +0x14 and not the source
 * link), message 5 clears +0x1C, message 6 runs the blast tick func_80036AB4,
 * messages 7-11 are forwarded to func_8002AD58 unless filtered by the link,
 * and all others go to func_8002AD58.
 */
s32 gHandleProjectileMsg(s32 arg0, struct Entity *arg1, s32 arg2, s32 arg3)
{
    register s32 message __asm__("$18");
    register struct Entity *self __asm__("$16");
    register struct Entity *other __asm__("$17");
    register s32 value __asm__("$19");
    u32 index;
    static void *const label_table[] __attribute__((used)) = {
        &&message_1, &&message_2, &&message_3, &&message_4, &&message_5,
        &&message_6, &&message_forward, &&message_forward, &&message_forward,
        &&message_forward, &&message_forward
    };

    message = arg0;
    self = arg1;
    other = (struct Entity *)arg2;
    value = arg3;
    index = (u32)message - 1;
    if (index >= 11) {
        goto fallback;
    }
    goto *jtbl_80011380[index];

message_1:
    return 0xD4;
message_2:
    func_80036940(self, (s32)other, value);
zero:
    return 0;
message_3:
    {
        register s32 dx __asm__("$2");
        register s32 dy __asm__("$3");
        register s32 sign __asm__("$4");

        func_80036BD4(self, (s32)other, value);
        dx = self->unk78.x;
        dy = self->unk78.y;
        sign = dx >> 31;
        dx = (sign + dx) ^ sign;
        sign = dy >> 31;
        dy = (sign + dy) ^ sign;
        if (dx + dy < 0x10) {
            gBuildBasisFromAxisYZ(&self->unk78, self->unk2C);
        } else {
            gBuildBasisFromAxis(&self->unk78, self->unk2C);
        }
        if (func_8001A750(&self->unk50, &self->unkC8) > 0x58000) {
            gRetireEntity(self);
        }
        gGteLoadMatrix(self->unk2C);
        {
            register s32 vertex_offset __asm__("$4");
            register u8 *vertices __asm__("$2");

            vertices = self->unk28->unk34;
            vertex_offset = self->unk70 << 4;
            gGteTransformVerticesWide(vertices + 8 + vertex_offset, self->unk5C, 1);
        }
        return 0;
    }
message_4:
    {
        s32 different;

        different = 0;
        if (self->unk14 != other->unk14) {
            different = self->unkC0 != (struct EntityLink *)other;
        }
        return different;
    }
message_5:
    self->unk1C = -1;
    return 0;
message_6:
    func_80036AB4(self, value);
    goto fallback;
message_forward:
    if (other->unk08 != 0) {
        if (!(other->unk0C & 1) || other->unk08->unk18 != self->unk18) {
            gEntityBaseHandler(message, self, (s32)other, value);
        }
    }
    return 0;
fallback:
    return gEntityBaseHandler(message, self, (s32)other, value);
}
