#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Matrix {
    s32 m[12];
};

struct Source {
    u8 pad00[4];
    u8 kind;
    u8 pad05[0x0F];
    u8 unk14;
    u8 childType;
    u8 pad16[0x08];
    s8 unk1E;
    u8 pad1F[0x05];
    s32 unk24;
    u8 pad28[0x06];
    u16 unk2E;
    s16 unk30;
    u16 flags32;
};

struct Object {
    struct Object *next;
    u8 pad04[2];
    u8 unk06;
    u8 pad07[0x0E - 0x07];
    u8 unk0E;
    u8 flags0F;
    u8 flags10;
    u8 soundId11;
    u16 unk12;
    u8 pad14[0x1A - 0x14];
    u16 flags1A;
    void *parent1C;
    struct Source *source20;
    s32 (*func24)(s32, struct Object *, void *, s32);
    u8 pad28[0x34 - 0x28];
    struct Matrix matrix;
};

struct Parent {
    u8 pad00[0x34];
    void *holder34;
    u8 pad38;
    u8 flags39;
    u16 unk3A;
    u8 pad3C[2];
    u16 unk3E;
};

struct Data {
    u8 pad00[0x24];
    s32 x;
    s32 y;
    s32 z;
};

struct Player {
    u8 pad00[0x50];
    s32 x;
    s32 y;
    s32 z;
};

struct Target {
    u8 pad00[0x28];
    s32 x;
    s32 y;
    s32 z;
};

struct Holder;

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct SpawnWork {
    struct Matrix matrix;       /* +00: retail sp+20 */
    u16 half;                   /* +30: retail sp+50 */
    s8 slot;                    /* +32: retail sp+52 */
    u8 pad33[5];
    s32 diff[4];                /* +38: retail sp+58 */
    struct DestinationTriple point; /* +48: retail sp+68 */
    s32 point_pad;
    s32 distVec[4];             /* +58: retail sp+78 */
    s32 vec1[4];                /* +68: retail sp+88 */
    s32 vec2[4];                /* +78: retail sp+98 */
};

extern void *D_800A7040[][0x100];
extern s32 D_8009FA0C;
extern struct Player *D_8009FA58;
extern struct Player *D_8009FA5C;
extern void *D_8009F374;
extern struct Object *D_8009F314;

extern void func_8003DB70(struct Source *, struct Object *, struct Parent *, void *, struct Matrix *, u16 *, s8 *);
extern struct Data *func_8003D9F8(struct Holder *, u16, s8, s32, struct Parent *, struct Matrix *);
extern u32 *func_8003B3D8(u32, u32);
extern void func_8003B3F8(struct Object *, u32, s32);
extern s32 func_8003CDAC(struct Holder *, s8);
extern void func_80069580(u32, struct DestinationTriple *);
extern s32 func_8001A100(s32 *);
extern s32 func_80019F54(s32 *);
extern void func_80057358(void *, s32);
extern void func_80025D6C(s32 *);
extern void func_800410BC(s32 *, u16, s16, void *);
extern struct Object *func_80040E60(struct Source *);
extern struct Object *func_8007043C(struct Source *);
extern struct Object *func_8006EB80(struct Source *);
extern struct Object *func_800447F0(struct Source *);
extern struct Object *func_8004C920(struct Source *);
extern struct Object *func_800762B0(struct Source *);
extern struct Object *func_80074024(struct Source *);
extern s32 func_80046C84(s32, s32);
extern void func_80047284(s32, s32, s32, s32);

s32 func_8003E984(struct Parent *parent, struct Object *link, u8 type, void *arg3, s32 arg4)
{
    struct SpawnWork work;
    register struct Source *source __asm__("$21");
    register struct Data *data __asm__("$20");
    register struct Object *object __asm__("$18");
    struct Object *prev;
    struct Object *tmp;
    register struct Holder *holder __asm__("$19");
    struct Target *target;
    u8 found;
    register s32 i __asm__("$16");
    register s32 amount __asm__("$22");
    s32 dist;
    s32 dist2;
    register s32 last_matrix_word __asm__("$8");

    source = (struct Source *)D_800A7040[arg4][type];
    if (source == 0) {
        goto failed;
    }

    holder = 0;
    if (parent != 0) {
        holder = (struct Holder *)parent->holder34;
    }

    func_8003DB70(source, link, parent, arg3, &work.matrix, &work.half, &work.slot);
    data = func_8003D9F8(holder, source->flags32, source->unk1E, 0, parent, &work.matrix);

    if ((*(u8 *)func_8003B3D8(type, arg4) & 5) == 0) {
        found = 0;
        for (i = 0; i < D_8009FA0C; i++) {
            func_80069580(i, &work.point);
            work.diff[0] = data->x - work.point.second;
            work.diff[1] = data->y - work.point.zero;
            work.diff[2] = data->z - work.point.first;
            if (func_8001A100(work.diff) <= 0x1FFFF) {
                found = 1;
            }
        }
        if (!found) {
            goto failed;
        }
    }

    switch (source->kind) {
    case 0x5D:
        tmp = func_80040E60(source);
        break;
    case 0x59:
        tmp = func_8007043C(source);
        break;
    case 0x5C:
        tmp = func_8006EB80(source);
        break;
    case 0x5E:
        tmp = func_800447F0(source);
        break;
    case 0x5A:
        tmp = func_8004C920(source);
        break;
    case 0x57:
        tmp = func_800762B0(source);
        break;
    case 0x58:
        tmp = func_80074024(source);
        break;
    case 0x5B:
    default:
        tmp = 0;
        break;
    }
    object = tmp;
    if (object == 0) {
        goto failed;
    }

    object->source20 = source;
    func_8003B3F8(object, type, arg4);
    if (arg4 == 1) {
        object->flags10 |= 2;
    }

    object->parent1C = parent;
    if (parent != 0) {
        parent->unk3A++;
        if (!(object->flags10 & 4)) {
            parent->unk3E++;
        }
    }

    if (holder != 0) {
        if ((u8)func_8003CDAC(holder, work.slot) == 0) {
            work.slot = 0xFF;
        }
    }
    object->unk0E = work.slot;

    object->matrix.m[0] = work.matrix.m[0];
    object->matrix.m[1] = work.matrix.m[1];
    object->matrix.m[2] = work.matrix.m[2];
    object->matrix.m[3] = work.matrix.m[3];
    object->matrix.m[4] = work.matrix.m[4];
    object->matrix.m[5] = work.matrix.m[5];
    object->matrix.m[6] = work.matrix.m[6];
    object->matrix.m[7] = work.matrix.m[7];
    object->matrix.m[8] = work.matrix.m[8];
    object->matrix.m[9] = work.matrix.m[9];
    object->matrix.m[10] = work.matrix.m[10];
    last_matrix_word = work.matrix.m[11];
    object->unk12 = work.half;
    object->matrix.m[11] = last_matrix_word;

    if (object->func24(0, object, arg3, 0) == 0) {
        object->func24(10, object, 0, 0);
        goto failed;
    }

    if (object->flags1A & 0x2010) {
        amount = (object->flags1A & 0x2000) ? 0x3E8 : 0x12C;

        work.distVec[0] = D_8009FA58->x - data->x;
        work.distVec[1] = D_8009FA58->y - data->y;
        work.distVec[2] = D_8009FA58->z - data->z;
        if (ABS(func_80019F54(work.distVec)) <= 0x1FFFF) {
            func_80057358(D_8009FA58, amount);
        }

        if (D_8009FA0C >= 2) {
            work.distVec[0] = D_8009FA5C->x - data->x;
            work.distVec[1] = D_8009FA5C->y - data->y;
            work.distVec[2] = D_8009FA5C->z - data->z;
            if (ABS(func_80019F54(work.distVec)) <= 0x1FFFF) {
                func_80057358(D_8009FA5C, amount);
            }
        }
    }

    if (object->flags1A & 0x20) {
        func_80025D6C(&data->x);
    }

    if (object->source20->unk2E != 0) {
        if (object->source20->unk30 > 0) {
            func_800410BC(&data->x, object->source20->unk2E, object->source20->unk30, D_8009F374);
        } else {
            object->func24(9, object, 0, 0);
        }
    }

    prev = D_8009F314;
    D_8009F314 = object;
    object->next = prev;

    if (object->soundId11 != 0) {
        target = 0;
        if (object->parent1C != 0) {
            target = (struct Target *)((struct Parent *)object->parent1C)->holder34;
        }

        if ((type >= 0x3D && type < 0x45) || (type >= 0x64 && type < 0x77)) {
            if (D_8009FA0C >= 2) {
                if (target != 0) {
                    work.vec1[0] = target->x - D_8009FA58->x;
                    work.vec1[1] = target->y - D_8009FA58->y;
                    work.vec1[2] = target->z - D_8009FA58->z;
                    work.vec2[0] = target->x - D_8009FA5C->x;
                    work.vec2[1] = target->y - D_8009FA5C->y;
                    work.vec2[2] = target->z - D_8009FA5C->z;
                } else {
                    work.vec1[0] = object->matrix.m[9] - D_8009FA58->x;
                    work.vec1[1] = object->matrix.m[10] - D_8009FA58->y;
                    work.vec1[2] = object->matrix.m[11] - D_8009FA58->z;
                    work.vec2[0] = object->matrix.m[9] - D_8009FA5C->x;
                    work.vec2[1] = object->matrix.m[10] - D_8009FA5C->y;
                    work.vec2[2] = object->matrix.m[11] - D_8009FA5C->z;
                }
                dist = func_80019F54(work.vec1);
                dist2 = func_80019F54(work.vec2);
                if (dist < dist2) {
                    if (dist <= 0x1FFFF) {
                        func_80046C84(object->soundId11, (s32)D_8009FA58);
                    } else if (dist2 <= 0x1FFFF) {
                        func_80046C84(object->soundId11, (s32)D_8009FA5C);
                    }
                }
            } else {
                if (target != 0) {
                    work.vec1[0] = target->x - D_8009FA58->x;
                    work.vec1[1] = target->y - D_8009FA58->y;
                    work.vec1[2] = target->z - D_8009FA58->z;
                } else {
                    work.vec1[0] = object->matrix.m[9] - D_8009FA58->x;
                    work.vec1[1] = object->matrix.m[10] - D_8009FA58->y;
                    work.vec1[2] = object->matrix.m[11] - D_8009FA58->z;
                }
                dist = func_80019F54(work.vec1);
                if (dist <= 0x1FFFF) {
                    func_80046C84(object->soundId11, (s32)D_8009FA58);
                }
            }
        } else {
            if (target != 0) {
                func_80047284(object->soundId11, target->x, target->z, -1);
            } else {
                func_80047284(object->soundId11, object->matrix.m[9], object->matrix.m[11], -1);
            }
        }
    }

    if (parent != 0) {
        if (object->flags1A & 0x40) {
            parent->flags39 |= 8;
        }
    }

    if (link != 0 && link->source20->unk14 == type && link->unk06 == 0) {
        goto success;
    }

    if (source->unk24 != 0) {
        goto success;
    }
    if (source->childType == 0) {
        goto success;
    }
    if (source->flags32 & 0x800) {
        goto success;
    }

    func_8003E984(parent, object, source->childType, 0, arg4);
    object->unk06 = 0;
    object->flags0F |= 2;
success:
    return (s32)object;
failed:
    return 0;
}
