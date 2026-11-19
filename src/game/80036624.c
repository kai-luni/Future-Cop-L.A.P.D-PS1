// Categories: Gameplay > Entity Script | Gameplay > Combat | Game Engine > Entity Lifecycle
#include "game/types.h"

extern int gSpawnQueuedEntity();
extern int gSpawnDirectShot();
extern int gWeaponFireDispatch();

struct Entity {
    u8 pad00[8];
    struct Entity *link;
    u16 flags;
    u8 pad0e[6];
    u16 id;
    u8 type;
    u8 pad17;
    u8 kind;
    u8 pad19;
    s16 val1a;
    s16 val1c;
    u8 pad1e[0x32];
    s32 pos[3];
    u8 pad5c[0x44];
    s16 x, y, z, radius;
    u8 pada8[0xc];
    struct Entity *owner;
    u8 padb8[0x10];
    struct Entity *other;
    u8 padcc[4];
    s16 strength;
    u8 padd2[0xa];
    u8 mode;
    u8 paddd[3];
    s32 state;
    u32 flags2;
    u8 pade8[4];
    struct Entity *target;
};
extern void gProjectileInit(struct Entity *, struct Entity *, s32) __asm__("func_80035904");
extern void gGuidedProjectileUpdate(struct Entity *, struct Entity *) __asm__("func_80035B7C");
extern s32 gEntityBaseHandler(s32, struct Entity *, struct Entity *, s32) __asm__("func_8002AD58");
extern s32 func_80019FCC(s32 *);
extern s32 gSqrt12(s32) __asm__("func_80095900");
extern void func_800410BC(void *, s32, s32, struct Entity *);

/**
 * @brief Message table of the model-backed entity class (block size 0xFC): spawn,
 *        per-frame mover tick, candidate test, hit absorption and a distance-scaled
 *        radial blast.
 *
 * @param message Handler case: 1, 2, 4, 5, 6, 9, 10, 11.
 * @param self Entity block: linked record +0x08, flag word +0x0C, id +0x14, type
 *        +0x16, kind +0x18, amount words +0x1A and +0x1C, position +0x50, extent
 *        words +0xA8/+0xAA/+0xAC, radius +0xAE, owner +0xBC, other +0xD0, strength
 *        +0xD8, mode +0xE4, state +0xE8, flag word +0xEC, target +0xF4.
 * @param data Case payload: the spawn descriptor for 2, another entity for 9, 10
 *        and 11, the time step for 6.
 * @param extra Forwarded to the base handler and to gProjectileInit.
 * @return Case 1: 0xFC. Cases 2, 6, 9, 11 and 5: 0. Case 10: 1 when the candidate is
 *         acceptable, 0 when it is not. Case 4: the base handler result. Default:
 *         the base handler result.
 *
 * @par Confirmed
 * 1 returns 0xFC, the size the spawn queue asks for before it allocates.
 * 2 runs gProjectileInit(self, data, extra), the spawn half, and returns 0.
 * 6 runs gGuidedProjectileUpdate(self, data), the mover tick, and returns 0.
 * 10 is the candidate test. The stored other (+0xD0) is first resolved through one
 * indirection: for type 0x12 through its +0xBC owner, for type 0x15 through its
 * +0xF4 target, and the candidate is refused when that owner equals the payload. A
 * payload of type 2 whose kind byte matches self->+0x18 is refused, an entity whose
 * 14-bit id matches the stored other's is refused, and otherwise the answer is
 * "the stored other is not the payload".
 * 11 is the hit case. A payload of type 14 with bit 3 of its byte +0xA0 set is
 * refused. With mode 0x15 the entity state becomes 4 and +0x1C is reduced by
 * max(payload +0x1A, payload +0x1C, 2); every other mode sets +0x1C to -1.
 * 4 is the burst case, and it falls through to the base handler afterwards. With
 * strength (+0xD8) above zero and bit 0 of +0xEC set, the distance from
 * (the three extent words shifted left by 8, at +0xA8) to the position at +0x50 is
 * measured with func_80019FCC and turned into a length by func_80095900, and the
 * amount +0x1A is reduced in proportion to that length over the radius +0xAE;
 * without the flag the amount is +0x1A unchanged. A positive amount fires
 * func_800410BC(position, amount, strength << 8, other).
 * 5 returns 0 without consulting the base handler.
 * 9 forwards to the base handler only when the payload has a linked record at +0x08
 * and it is not the case that the payload has bit 0 of its +0x0C flag word and a link
 * whose kind matches self->+0x18; it returns 0 either way.
 *
 * @par Likely role
 * The dispatcher of a mobile, model-backed unit: message 2 binds the definition's
 * model and animation, message 6 is a full flight tick with target acquisition and
 * collision, message 10 is the "may I target this" question the AI asks of its
 * peers, message 11 is the contact reaction that absorbs the other body's amount,
 * and message 4 is the burst that damages everything inside the radius +0xAE with a
 * linear falloff.
 *
 * @par Unknown
 * - What the strength at +0xD8 and the radius at +0xAE measure in game terms; only
 *   their use inside the blast is visible.
 * - What mode 0x15 means and why every other mode stores -1 in +0x1C.
 * - The units of the 14-bit id compare and whether the +0xA0 bit 3 of the payload
 *   marks an invulnerable or already-consumed object.
 *
 * @par Callers
 * No direct callers in the call graph. gSpawnDirectShot stores the address as
 * request.handler and passes the request to gSpawnQueuedEntity, which allocates the block
 * from the message-1 size and then drives messages 1 and 2; gSpawnDirectShot itself is
 * called by gGuidedProjectileUpdate (child creation), func_8005F6E8 and gWeaponFireDispatch.
 */
s32 func_80036624(s32 msg, struct Entity *self, struct Entity *data, s32 extra)
{
    s32 vec[3];
    s32 v, diff;
    s16 amount;
    struct Entity *other;
    struct Entity *owner;
    register struct Entity *incoming __asm__("$5");
    switch (msg) {
    case 1:
        return 0xfc;
    case 2:
        gProjectileInit(self, data, extra);
        return 0;
    case 6:
        gGuidedProjectileUpdate(self, data);
        return 0;
    case 10:
        other = self->other;
        incoming = data;
        if (other) {
            switch (other->type) {
            case 0x12:
                owner = other->owner;
                goto check_owner;
            case 0x15:
                owner = other->target;
            check_owner:
                if (owner == data) return 0;
            }
        }
        if (incoming->type == 2 && incoming->kind == self->kind) return 0;
        if (self->other && (self->other->id & 0x3fff) == (incoming->id & 0x3fff)) return 0;
        return self->other != incoming;
    case 11:
        if (data->type == 14 && (data->pad5c[0x44] & 8)) return 0;
        if (self->mode == 0x15) {
            v = data->val1c;
            diff = v - data->val1a;
            self->state = 4;
            v -= (diff >> 31) & diff;
            diff = v - 2;
            v -= (diff >> 31) & diff;
            self->val1c -= v;
        } else {
            self->val1c = -1;
        }
        return 0;
    case 9:
        if (!data->link) return 0;
        if ((data->flags & 1) && data->link->kind == self->kind) return 0;
        gEntityBaseHandler(msg, self, data, extra);
        return 0;
    case 4:
        if (self->strength > 0) {
            if (self->flags2 & 1) {
                vec[0] = (self->x << 8) - self->pos[0];
                vec[1] = (self->y << 8) - self->pos[1];
                vec[2] = (self->z << 8) - self->pos[2];
                amount = self->val1a - (((gSqrt12(func_80019FCC(vec)) << 2) >> 8) * self->val1a / self->radius);
            } else {
                amount = self->val1a;
            }
            if (amount > 0) func_800410BC(self->pos, amount, self->strength << 8, self->other);
        }
        break;
    case 5:
        return 0;
    }
    return gEntityBaseHandler(msg, self, data, extra);
}
