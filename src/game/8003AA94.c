#include "game/types.h"

typedef struct { s32 x, y, z; } Vec;
typedef struct { s32 m[9]; s32 x, y, z; } Matrix;
typedef struct {
    u8 pad[3];
    u8 start, count;
    u8 pad5[2];
    u8 flags;
    s16 x, y, z, rx, ry, rz;
} Entry;
typedef struct { u32 flags; u8 a, b, c, d; u8 pad[4]; } Polygon;
typedef struct { s32 count; Polygon poly[1]; } Polygons;
typedef struct {
    u8 pad[0x14];
    Entry *entries;
    s16 *table;
    u8 pad1c[8];
    Polygons *polygons;
} Model;
typedef struct { s32 data[19]; } Copy;
typedef struct { s32 data[11]; } Copy11;
typedef struct { s32 a[6]; } Copy6;
typedef struct { s32 a[5]; } Copy5;
typedef struct {
    u8 pad00[0xc];
    u8 clear[8];
    u16 bits;
    u8 type;
    u8 pad17[9];
    u8 b20, b21;
    s8 b22;
    u8 b23;
    u32 flags;
    Model *model;
    u8 pad2c[0x30];
    Vec position;
    u8 pad68[8];
    s16 tableOffset;
    u8 flag72;
    u8 pad73[0x11];
    s32 field84;
    u8 pad88[10];
    u8 b92, b93;
    s32 field94, field98, field9c;
    u8 ba0, ba1, ba2, padA3;
    s16 fielda4;
    u8 padA6[2];
    u32 mask[8];
    void *particles;
} Object;
typedef struct {
    s16 zero0, zero2, zero4;
    s16 vx, vy, vz;
    s16 zeroC, zeroE;
    s16 extra;
    u8 a, b;
} Particle;
typedef struct { Object *source; void *force; void *force2; u8 value; } Config;
typedef struct { u8 pad[8]; Config *config; } Arg;

extern Matrix D_8009AA38;
extern void *func_80017EBC(s32, s32);
extern s32 func_80019668(void);
extern Entry *func_8003A90C(Entry *, s32);
extern s32 func_8004B244(void *, void *, Vec *);
extern void func_8004B310(void *, Vec *, Vec *, s32, s16 *, s16 *, s32 *, Vec *);
extern void func_80018DC0(s32, s32, s32, Matrix *);
extern void func_80010820(Matrix *);
extern void func_8001006C(Matrix *, Matrix *, s32);
extern s32 func_80066F78(Vec *, s32, s32, s32);

s32 func_8003AA94(s32 unused, Object *out, Arg *arg)
{
    Config *config;
    s32 count;
    Model *model;
    Object *source;
    Entry *entry;
    Particle *particle;
    s16 *table;
    s32 offset;
    s32 i;
    u8 flags;
    s32 parentIndex;
    s32 rx, ry, rz;
    Vec force;
    Vec pos;
    Vec velocity;
    Matrix matrix;
    Matrix matrices[6];
    s16 angle1, angle2;
    s32 extra;
    u8 forceEnabled;
    Matrix *parent, *dest;
    Polygon *poly;
    s32 lower, upper;
    s32 d;

    config = arg->config;
    source = config->source;
    out->particles = func_80017EBC(0xf0, 3);
    *(Copy *)&out->model = *(Copy *)&source->model;
    out->b22 = -1;
    out->ba0 = config->value;
    out->b20 = 0;
    out->b21 = 0;
    out->b23 = 0;
    out->flags = source->flags;
    out->flag72 &= ~0x10;
    out->bits = source->bits | 0x800;
    out->type = 4;
    out->b92 = source->b92;
    out->b93 = source->b93;
    out->field94 = 0;
    out->field98 = 0;
    out->field9c = 0;
    out->field84 = 0;
    out->flags = (out->flags | 0x120001d) & 0xffbfdfff;
    for (i = 3; i >= 0; i--) out->clear[i * 2] = 0;
    count = 0;
    model = out->model;
    particle = out->particles;
    entry = model->entries;
    if (config->force) {
        matrices[0].m[1] = D_8009AA38.m[1];
        matrices[0].m[2] = D_8009AA38.m[2];
        matrices[0].m[3] = D_8009AA38.m[3];
        matrices[0].m[4] = D_8009AA38.m[4];
        matrices[0].m[5] = D_8009AA38.m[5];
        matrices[0].m[6] = D_8009AA38.m[6];
        matrices[0].m[7] = D_8009AA38.m[7];
        matrices[0].m[8] = D_8009AA38.m[8];
        matrices[0].x = D_8009AA38.x;
        matrices[0].y = D_8009AA38.y;
        matrices[0].z = D_8009AA38.z;
        matrices[0].m[0] = D_8009AA38.m[0];
        table = model->table;
        offset = out->tableOffset;
        forceEnabled = func_8004B244(config->force, config->force2, &force);
        do {
            particle->zero0 = 0;
            particle->zero2 = 0;
            particle->zero4 = 0;
            particle->zeroC = 0;
            particle->zeroE = 0;
            count++;
            flags = entry->flags;
            parentIndex = entry->pad[0];
            if (flags & 4) rx = entry->rx; else rx = table[offset + entry->rx];
            if (flags & 2) ry = entry->ry; else ry = table[offset + entry->ry];
            if (flags & 1) rz = entry->rz; else rz = table[offset + entry->rz];
            func_80018DC0(rx, ry, rz, &matrix);
            if (flags & 0x20) matrix.x = entry->x << 4; else matrix.x = table[offset + entry->x] << 4;
            if (flags & 0x10) matrix.y = entry->y << 4; else matrix.y = table[offset + entry->y] << 4;
            if (flags & 8) matrix.z = entry->z << 4; else matrix.z = table[offset + entry->z] << 4;
            parent = &matrices[parentIndex - 1];
            dest = &matrices[parentIndex];
            func_80010820(parent);
            func_8001006C(&matrix, dest, 4);
            dest->x += parent->x;
            dest->y += parent->y;
            dest->z += parent->z;
            pos.x = dest->x;
            pos.y = dest->y;
            pos.z = dest->z;
            func_8004B310(config->force, &pos, &force, forceEnabled, &angle1, &angle2, &extra, &velocity);
            particle->vx = velocity.x >> 4;
            particle->vy = velocity.y >> 4;
            particle->vz = velocity.z >> 4;
            particle->extra = extra >> 4;
            particle->a = (u16)angle1 >> 4;
            particle->b = (u16)angle1 >> 4;
            if (entry->flags & 0x80) break;
            entry++;
            particle++;
        } while (1);
    } else {
        do {
            particle->zero0 = 0;
            particle->zero2 = 0;
            particle->zero4 = 0;
            particle->zeroC = 0;
            particle->zeroE = 0;
            particle->vx = func_80019668() & 0x3f;
            particle->vy = (func_80019668() & 0xff) + 0x100;
            particle->vz = func_80019668() & 0x3f;
            particle->extra = -0x100;
            particle->a = func_80019668();
            particle->b = func_80019668();
            count++;
            if (entry->flags & 0x80) break;
            entry++;
            particle++;
        } while (1);
    }
    {
        s32 y = func_80066F78(&out->position, 0, 0, 0);
        out->ba2 = count;
        out->fielda4 = (y - out->position.y) >> 3;
        out->ba1 = count;
    }
    for (i = 7; i >= 0; i--) out->mask[i] = 0;
    i = model->polygons->count;
    poly = model->polygons->poly;
    while (i > 0) {
        entry = func_8003A90C(model->entries, poly->a);
        lower = entry->start;
        upper = lower + entry->count - 1;
        d = poly->a;
        if (d >= lower && d <= upper) {
        d = poly->b;
        if (d >= lower && d <= upper) {
        d = poly->c;
        if (d >= lower && d <= upper) {
            d = poly->d;
            if (((poly->flags >> 8) & 0xf) != 4 || (d >= lower && d <= upper)) {
                out->mask[i / 32] |= 1 << (i & 31);
            }
        }
        }
        }
        poly++;
        i--;
    }
    return 0;
}
