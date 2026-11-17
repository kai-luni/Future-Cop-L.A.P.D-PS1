// Categories: Physics > Spatial Queries
#include "game/types.h"

typedef struct { s32 x, y, z; } Vec;
typedef struct { s32 count, stride; } Table;
typedef struct { char pad[0x34]; Table *table; } Model;
typedef struct { Model *model; s32 matrix[12]; char pad[0x12]; s16 radius, index; } Transform;
typedef struct { char pad[0x24]; u32 flags; Transform transform; } Object;
typedef struct { Object *obj; u8 id; char pad[3]; } Entry;
extern s32 gQueueCount __asm__("D_8009F838");
extern Entry gObjectKeyTable[] __asm__("D_800A31A0");
extern s32 func_8001A750(s32 *, s32 *);
extern void func_80018860(void *, void *);
extern void func_8001A970(void *, void *, void *);

/**
 *  @brief Finds the first eligible object whose local box matches a position.
 *
 *  Candidate objects are filtered by flags and radius, then tested against model table boxes in local space.
 *  A match supplies a rounded world-space height and the entry identifier.
 *  @param pos World-space position to test.
 *  @param out Optional destination for the vector stored at matrix offset three.
 *  @param height Destination for the rounded world-space height.
 *  @param id Destination for the matching entry identifier.
 *  @return The first matching object, or null if none matches.
 *  @par Globals
 *  D_8009F838 gives the entry count; D_800A31A0 contains the object entries.
 *  @par Control flow
 *  Iterates entries and their boxes, skipping the first box when the table has at least two, and returns on the first match.
 *  @par Calls
 *  func_8001A750 performs the radius test; func_80018860 prepares a matrix; func_8001A970 transforms positions.
 *  @par Side effects
 *  Writes height, id, and optionally out only when a match is found.
 *  @par Callers
 *  Confirmed: func_80066F78.
 */
Object *func_8006320C(Vec *pos, Vec *out, s32 *height, u8 *id)
{
    register Entry *current __asm__("$20");
    s32 matrix[12];
    Vec local, result;
    s32 i, n, d, offset;
    Entry *entry;
    Object *obj;
    Transform *t;
    Model *model;
    Table *table;
    s16 *box;
    i = 0;
    if (i < gQueueCount) {
      entry = gObjectKeyTable;
      do {
        obj = entry->obj;
        if (!(obj->flags & 0x20000000)) goto next;
        t = &obj->transform;
        if (func_8001A750(&obj->transform.matrix[12], (s32 *)pos) >= (t->radius << 4)) goto next;
        func_80018860(&t->matrix, matrix);
        func_8001A970(matrix, pos, &local);
        offset = (t->index << 4) + 8;
        model = t->model;
        table = model->table;
        box = (s16 *)((char *)table + offset);
        n = table->count;
        current = entry;
        if (n >= 2) { box = (s16 *)((char *)box + table->stride); n--; }
        do {
            d = local.x - (box[0] << 4);
            if (((d >> 31) + d ^ (d >> 31)) <= (box[3] << 4)) {
                d = local.z - (box[2] << 4);
                if (((d >> 31) + d ^ (d >> 31)) <= (box[5] << 4)) {
                    if (local.y + 0x200 >= ((box[1] - box[4]) << 4)) {
                        local.y = (box[1] + box[4]) << 4;
                        func_8001A970(&t->matrix, &local, &result);
                        *height = (result.y + 0x80) & -0x100;
                        if (out) *out = *(Vec *)&t->matrix[3];
                        *id = current->id;
                        return obj;
                    }
                }
            }
            box = (s16 *)((char *)box + model->table->stride);
        } while (--n > 0);
next:
        entry++;
      } while (++i < gQueueCount);
    }
    return 0;
}
