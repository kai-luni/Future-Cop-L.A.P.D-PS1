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

extern Node **D_8009F780;
extern s32 D_8009F78C;

extern void func_800188D0(s32 angle, s32 arg7, s32 arg8, struct TransformMatrix *matrix);
extern void func_8001AF84(struct TransformMatrix *matrix, struct TransformPoint *source,
                          struct TransformPoint *destination, s32 count);
extern u16 func_80083764(s32 x, s32 y, s32 left, s32 right, s32 top, s32 bottom);

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

    index = (z >> 17) + D_8009F78C * (x >> 17);
    if (index != node->unk2) {
        if (node->unk2 >= 0) {
            curr = D_8009F780[node->unk2];
            if (curr == node) {
                D_8009F780[node->unk2] = node->next;
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
        if (node->unk4 >= 0 || D_8009F780[index] == 0) {
            node->next = D_8009F780[index];
            D_8009F780[index] = node;
        } else {
            curr = D_8009F780[index];
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
