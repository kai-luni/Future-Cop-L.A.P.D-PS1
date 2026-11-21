// Categories: Gameplay > Object Spawning | Graphics > Scene Graph | Graphics > Animation
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Basis {
    struct Vec3s32 rows[3];
};

struct Model {
    u8 pad00[0x10];
    u8 *anim;
    u8 pad14[0x20];
    u8 *unk34;
};

struct Instance {
    struct Model *model;
    struct Basis basis;
};

struct Self {
    u8 pad00[0x24];
    u32 flags;
    struct Instance inst;
    struct Vec3s32 pos;
    u8 unk5C[0x14];
    s16 unk70;
    u8 pad72[0x2E];
    u8 animTrack[0xC];
    s16 unkAC;
    u8 padAE[6];
    struct Thing *unkB4;
};

struct Thing {
    u8 pad00[0x100];
    s32 unk100;
};

struct Weapon {
    u8 pad00[4];
    u8 mode;
    u8 pad05[0x1F];
    s8 unk24;
    u8 unk25;
    s16 unk26;
    s32 unk28;
    u8 pad2C[8];
    s8 unk34;
    u8 pad35[5];
    s16 unk3A;
    u8 pad3C[8];
    u8 unk44;
    u8 pad45[1];
    s16 unk46;
};

struct Arg1 {
    u8 pad00[8];
    struct Weapon *weapon;
};

extern s32 gEntityBaseHandler(s32 message, struct Self *self, struct Arg1 *arg1, s32 arg2) __asm__("func_8002AD58");
extern s32 gQuerySurfaceHeight(struct Vec3s32 *pos, s32 sel) __asm__("func_80067684");
extern s32 gGetGroundHeight(struct Vec3s32 *pos, struct Vec3s32 *out, s32 a2, s32 a3) __asm__("func_80066F78");
extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
extern void gBuildBasisFromPair(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis) __asm__("func_800192AC");
extern void gSetAnimClipTable(u8 *data, u8 *track, u8 forward, s32 index) __asm__("func_80044988");
extern void gGteLoadMatrix(struct Basis *basis) __asm__("func_80010750");
extern void gGteTransformVerticesWide(u8 *ptr, u8 *unk5C, s32 a2) __asm__("func_80010234");
s32 gInitType11Entity(struct Self *self, struct Arg1 *arg1, s32 arg2) __asm__("func_8002DC94");


/**
 * @brief Spawn initializer for a weapon-derived object: reads the weapon
 *        mode, places the object on terrain and builds its yaw or
 *        slope-aligned basis.
 *
 * @param self Object being initialized.
 * @param arg1 Spawn argument; arg1->weapon is the weapon record.
 * @param arg2 Forwarded to the base initializer.
 * @return 0.
 *
 * @par Confirmed
 * Weapon mode 0x18 is remapped to 0xB. Modes 8, 0x21 and 0x24 take the
 * alternate fields (unk34, unk44, unk3A << 4, unk46) and set
 * flags = (flags | 0x11) & 0xFFEFDEF7; all other modes take unk24, unk25,
 * unk28, unk26. func_8002AD58(2, self, arg1, arg2) runs in both branches.
 * The mode maps to a terrain selector (0 -> -1, 1 -> -2, else mode - 2);
 * pos.y = func_80067684(&pos, sel) + base. With flag & 1 the height comes
 * from func_80066F78 instead and func_800192AC builds the basis from the
 * surface result and the direction (cos, 0, sin) of the angle
 * (func_8001846C); otherwise the basis is the explicit yaw matrix
 * [cos, 0, -sin; 0, 0x10000, 0; sin, 0, cos].
 * When the model has animation data, func_80044988 starts the track;
 * func_80010750 loads the basis, func_80010234 transforms one frame
 * vertex; unkB4 = 0.
 *
 * @par Callers
 * func_8002DEA8.
 *
 * @par Environment path
 * Spawn initializer of the type-11 entity, called only from message 2 of
 * func_8002DEA8. It remaps spawn mode 0x18 to 0xB, runs the base initializer
 * func_8002AD58(2, ...), and for modes 8, 0x21 and 0x24 takes the alternate
 * field set and sets the flag word to (flags | 0x11) & 0xFFEFDEF7. It maps
 * the terrain type to a selector, sets pos.y from func_80067684 plus the base
 * height, and either refines the height with func_80066F78 and builds a
 * slope-aligned basis with func_800192AC, or writes a plain yaw basis. It
 * finishes by starting the animation track (func_80044988), transforming one
 * frame vertex with gGteLoadMatrix and gGteTransformVerticesWide and clearing
 * the child pointer at +0xB4.
 */
s32 gInitType11Entity(struct Self *self, struct Arg1 *arg1, s32 arg2)
{
    struct Weapon *w;
    s32 mode;
    s32 sel;
    s32 base;
    u8 flag;
    s32 angle;
    struct Vec3s32 vec;
    struct Vec3s32 out;
    s32 sine;
    s32 cosine;
    s32 sine2;
    s32 cosine2;

    w = arg1->weapon;
    if (w->mode == 0x18) {
        w->mode = 0xB;
    }
    mode = w->mode;
    if (mode == 8 || mode == 0x21 || mode == 0x24) {
        gEntityBaseHandler(2, self, arg1, arg2);
        self->flags = (self->flags | 0x11) & 0xFFEFDEF7;
        mode = w->unk34;
        flag = w->unk44;
        base = w->unk3A << 4;
        angle = w->unk46;
    } else {
        gEntityBaseHandler(2, self, arg1, arg2);
        mode = w->unk24;
        base = w->unk28;
        flag = w->unk25;
        angle = w->unk26;
    }
    if (mode != 0) {
        if (mode == 1) {
            sel = -2;
        } else {
            sel = mode - 2;
        }
    } else {
        sel = -1;
    }
    self->pos.y = gQuerySurfaceHeight(&self->pos, sel) + base;
    if (flag & 1) {
        self->pos.y = gGetGroundHeight(&self->pos, &out, 0, 0) + base;
        gSinCos(angle, &sine, &cosine);
        vec.x = cosine;
        vec.y = 0;
        vec.z = sine;
        gBuildBasisFromPair(&out, &vec, &self->inst.basis);
    } else {
        gSinCos(angle, &sine2, &cosine2);
        self->inst.basis.rows[2].y = 0;
        self->inst.basis.rows[1].z = 0;
        self->inst.basis.rows[1].x = 0;
        self->inst.basis.rows[0].y = 0;
        self->inst.basis.rows[1].y = 0x10000;
        self->inst.basis.rows[2].z = cosine2;
        self->inst.basis.rows[2].x = sine2;
        self->inst.basis.rows[0].x = cosine2;
        self->inst.basis.rows[0].z = -sine2;
    }
    if (self->inst.model->anim != 0) {
        gSetAnimClipTable(self->inst.model->anim, self->animTrack, 1, 0);
    }
    gGteLoadMatrix(&self->inst.basis);
    gGteTransformVerticesWide(self->inst.model->unk34 + (((s32)self->unk70 << 4) + 8), self->unk5C, 1);
    self->unkB4 = 0;
    return 0;
}

