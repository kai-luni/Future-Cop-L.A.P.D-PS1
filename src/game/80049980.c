// Categories: Graphics > Lighting > Map Cell Light Node | Graphics > Culling | Physics > Collision
#include "game/types.h"

struct TransformMatrix {
    s32 values[12];
};

struct TransformPoint {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};

typedef struct Node {
    s16 unk0;
    s16 unk2;
    s32 unk4;
    u8 unk8;
    u8 pad9[9];
    u16 unk12;
    s32 unk14;
    struct TransformPoint points[5];
    struct Node *next;
} Node;

extern Node **gLightGrid __asm__("D_8009F780");
extern s32 gLightGridWidth __asm__("D_8009F78C");

extern void func_800188D0(s32 angle, s32 arg7, s32 arg8, struct TransformMatrix *matrix);
extern void func_8001AF84(struct TransformMatrix *matrix, struct TransformPoint *source,
                          struct TransformPoint *destination, s32 count);
extern u16 func_80083764(s32 x, s32 y, s32 left, s32 right, s32 top, s32 bottom);

/**
 * @brief Places and re-buckets a light node in the map cell grid: relinks it into
 *        its cell list, builds its depth key and its five corner points, and
 *        stores the 4x4 subcell coverage mask.
 *
 * @param node Node: +0x00 (an s16 this body never reads), +0x02 current cell
 *             (-1 = unlinked), +0x04 insertion key, +0x08 flag byte (bit 2 =
 *             flat record, bit 3 = angle stored), +0x12 subcell mask, +0x14
 *             depth, +0x18 five 8-byte points, +0x40 next pointer.
 * @param x World position on the axis that maps to the cell row.
 * @param z World position on the axis that maps to the cell column.
 * @param y Height.
 * @param width Extent along z.
 * @param depth Extent along x.
 * @param angle Yaw in the 0x1000-based angle unit.
 * @param arg7 Forwarded to func_800188D0 as its second argument.
 * @param arg8 Forwarded to func_800188D0 as its third argument.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. cell = (z >> 17) + D_8009F78C * (x >> 17), with D_8009F78C the grid width
 *    and D_8009F780 the array of cell list heads. When the cell changed the node
 *    is unlinked from the old list (head replacement or predecessor search) and
 *    inserted into the new one: at the head when +0x04 >= 0 or the list is empty,
 *    otherwise appended after the tail.
 * 2. +0x14 = depth - max(0, depth - width), the smaller of the two extents.
 * 3. Flag 4 (flat record): the points are written directly - point 0 gets
 *    (x & 0x1FFFF) >> 4, y >> 4 and (z & 0x1FFFF) >> 4, point 1 gets width >> 4
 *    and depth >> 4, and the angle goes to point 1 z with flag 8 set (or cleared
 *    when the angle is 0).
 * 4. Otherwise a matrix is built with y at +0x28, x & 0x1FFFF at +0x24 and
 *    z & 0x1FFFF at +0x2C, func_800188D0(angle, arg7, arg8, &matrix) adds the
 *    rotation, the five points become the centre plus the diamond
 *    (+-width/2, 0, +-depth/2) in 1/16 units, and
 *    func_8001AF84(&matrix, points, &node->+0x18, 5) transforms all five.
 * 5. The coverage mask is stored in +0x12:
 *    func_80083764(z, x, z - width / 2, z + width / 2, x - depth / 2,
 *    x + depth / 2), i.e. the cell origin is passed as (z, x).
 *
 * @par Likely role
 * The per-frame update of a world light: it keeps the light in the cell list that
 * the map renderer walks, derives the depth used for sorting and stores the
 * subcell mask so that only the cells the light can reach are shaded. The
 * deliberate (z, x) argument order and the flat-record path both point at the
 * light system - the light update func_800753D0 and the object walkers
 * func_80040A00 and func_80049C38 are the only callers.
 *
 * @par Unknown
 * The unit of +0x14 (sort key or radius) and what the two modes of flag 4
 * distinguish: the flat path stores a small record, the other one a full
 * transformed diamond, so the flag likely separates static from rotated lights.
 *
 * @par Callers
 * func_80040A00, func_80049C38 and func_800753D0.
 */
void func_80049980(Node *node, s32 x, s32 z, s32 y, s32 width, s32 depth,
                   s32 angle, s32 arg7, s32 arg8)
{
    int index;
    Node *curr;
    s32 diff;
    struct TransformMatrix matrix;
    struct TransformPoint points[5];
    s32 w32;
    s32 d32;

    index = (z >> 17) + gLightGridWidth * (x >> 17);
    if (index != node->unk2) {
        if (node->unk2 >= 0) {
            curr = gLightGrid[node->unk2];
            if (curr == node) {
                gLightGrid[node->unk2] = node->next;
            } else {
                while (curr != 0) {
                    if (curr->next == node) {
                        curr->next = node->next;
                        break;
                    }
                    curr = curr->next;
                }
            }
        }

        node->unk2 = index;
        if (node->unk4 >= 0 || gLightGrid[index] == 0) {
            node->next = gLightGrid[index];
            gLightGrid[index] = node;
        } else {
            curr = gLightGrid[index];
            while (curr->next != 0) {
                curr = curr->next;
            }
            curr->next = node;
            node->next = 0;
        }
    }

    diff = depth - width;
    node->unk14 = depth - ((diff >> 31) & diff);

    if (node->unk8 & 4) {
        node->points[0].x = (x & 0x1FFFF) >> 4;
        node->points[0].z = (z & 0x1FFFF) >> 4;
        node->points[0].y = y >> 4;
        node->points[1].x = width >> 4;
        node->points[1].y = depth >> 4;
        if (angle != 0) {
            node->points[1].z = angle;
            node->unk8 |= 8;
        } else {
            node->unk8 &= ~8;
        }
    } else {
        matrix.values[10] = y;
        matrix.values[9] = x & 0x1FFFF;
        matrix.values[11] = z & 0x1FFFF;
        func_800188D0(angle, arg7, arg8, &matrix);
        w32 = (width / 2) >> 4;
        points[1].z = w32;
        points[2].z = w32;
        points[4].z = -w32;
        points[3].z = -w32;
        d32 = (depth / 2) >> 4;
        points[2].x = d32;
        points[3].x = d32;
        points[4].x = -d32;
        points[1].x = -d32;
        points[4].y = 0;
        points[3].y = 0;
        points[2].y = 0;
        points[1].y = 0;
        points[0].z = 0;
        points[0].y = 0;
        points[0].x = 0;
        func_8001AF84(&matrix, points, &node->points[0], 5);
    }

    node->unk12 = func_80083764(z, x, z - width / 2, z + width / 2, x - depth / 2, x + depth / 2);
}
