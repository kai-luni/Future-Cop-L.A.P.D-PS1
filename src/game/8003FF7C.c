#include "game/types.h"

struct Vec4 {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
};

struct Work {
    struct Vec4 base1;
    struct Vec4 base2;
    struct Vec4 delta;
};

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct Active;
struct Other;

struct Node {
    struct Node *next;
    u8 type;
    u8 pad05[3];
    s32 arg08;
    u8 pad0C[4];
    struct Active *active;
    s32 id;
    struct Other *other;
    u8 flags;
    u8 pad1D[3];
};

struct Active {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x44];
    s32 x;
    s32 y;
    s32 z;
};

struct Other {
    u8 pad00[0x10];
    s32 x;
    s32 y;
    s32 z;
    u8 pad1C[0x2D];
    u8 flags;
};

struct TableEntry {
    u8 pad00[8];
    s32 x;
    s32 y;
    s32 z;
};

extern struct Node *D_8009F30C;
extern s32 D_8009FA0C;
extern struct TableEntry *D_800A7040[][0x100];

extern void func_800181D4(struct Node *node);
extern s32 func_8001A100(s32 *vector);
extern void func_8003FE0C(struct Node *node);
extern void func_8003FE90(struct Node *node);
extern void func_80069580(u32 index, struct DestinationTriple *destination);

void func_8003FF7C(void)
{
    struct Work work;
    struct Node *node;
    struct Node *prev;
    struct Node *next;
    struct TableEntry *entry;
    u8 in_range;

    func_80069580(0, (struct DestinationTriple *)&work.base1);
    prev = 0;

    if (D_8009FA0C >= 2) {
        func_80069580(1, (struct DestinationTriple *)&work.base2);
    }

    node = D_8009F30C;
    while (node != 0) {
        entry = D_800A7040[node->arg08][node->type];
        next = node->next;

        if (entry != 0) {
            if (node->active != 0) {
                if (node->active->unk08 != node->id) {
                    if (prev != 0) {
                        prev->next = next;
                    } else {
                        D_8009F30C = next;
                    }
                    node->next = 0;
                    func_800181D4(node);
                    node = next;
                    continue;
                }
                work.delta.x = node->active->x - work.base1.x;
                work.delta.y = node->active->y - work.base1.y;
                work.delta.z = node->active->z - work.base1.z;
            } else if (node->other != 0 && (node->other->flags & 4)) {
                work.delta.x = node->other->x - work.base1.x;
                work.delta.y = node->other->y - work.base1.y;
                work.delta.z = node->other->z - work.base1.z;
            } else {
                work.delta.x = entry->x - work.base1.x;
                work.delta.y = entry->y - work.base1.y;
                work.delta.z = entry->z - work.base1.z;
            }

            in_range = func_8001A100(&work.delta.x) <= 0x1FFFF;

            if (D_8009FA0C >= 2) {
                if (node->active != 0) {
                    work.delta.x = node->active->x - work.base2.x;
                    work.delta.y = node->active->y - work.base2.y;
                    work.delta.z = node->active->z - work.base2.z;
                } else if (node->other != 0 && (node->other->flags & 4)) {
                    work.delta.x = node->other->x - work.base2.x;
                    work.delta.y = node->other->y - work.base2.y;
                    work.delta.z = node->other->z - work.base2.z;
                } else {
                    work.delta.x = entry->x - work.base2.x;
                    work.delta.y = entry->y - work.base2.y;
                    work.delta.z = entry->z - work.base2.z;
                }

                if (func_8001A100(&work.delta.x) <= 0x1FFFF) {
                    in_range = 1;
                }
            }

            if (node->flags & 1) {
                if (in_range == 0) {
                    func_8003FE90(node);
                }
            } else {
                if (in_range != 0) {
                    func_8003FE0C(node);
                }
            }

            prev = node;
        }

        node = next;
    }
}
