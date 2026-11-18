// Categories: Gameplay > Object Spawning | Gameplay > Map | Game Engine > Object Setup
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

extern struct Node *gNodeCList __asm__("D_8009F30C");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern struct TableEntry *gSpawnRecordMap[][0x100] __asm__("D_800A7040");

extern void gFreeBlock(struct Node *node) __asm__("func_800181D4");
extern s32 func_8001A100(s32 *vector);
extern void func_8003FE0C(struct Node *node);
extern void func_8003FE90(struct Node *node);
extern void func_80069580(u32 index, struct DestinationTriple *destination);

/**
 * @brief Proximity gate of the map's spawn nodes: instantiates the objects whose
 *        cells lie near a player's ground position and tears down the ones that
 *        drifted away or whose instance was replaced.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_80069580(0, &work.base1) always fetches the first stored ground position (the
 * D_800A3190 pair, middle component zeroed) and func_80069580(1, &work.base2) does
 * the same for the second one when D_8009FA0C >= 2 (two views/players). D_8009F30C
 * is then walked with next captured first; node +0x08 selects the row and node +0x04
 * the column of D_800A7040, and a null cell record skips the node.
 * A node whose instance word +0x10 is non-null but whose +0x08 no longer equals the
 * node's +0x14 id is stale: it is unlinked (previous->next or the list head) and
 * freed with func_800181D4. Otherwise the comparison position is the instance's
 * +0x50/+0x54/+0x58, else the +0x18 record's +0x10/+0x14/+0x18 when its +0x3D byte
 * has bit 4, else the cell record's +0x8/+0xC/+0x10; the vector length comes from
 * func_8001A100 and the cell counts as in range at 0x1FFFF or below. With
 * D_8009FA0C >= 2 the same test against base2 can also set the range flag.
 *
 * Actions:
 * The node flags byte +0x1C bit 0 is the "live" marker: with it set and the node out
 * of range, func_8003FE90 walks the seven D_800A1070 buckets, unlinks every object
 * whose +0x44 owner is that node, sends them message 10 and clears the node's bit 0;
 * with it clear and the node in range, func_8003FE0C builds the object through
 * func_8003D680(node->source, type, +0x08, +0x18, 0, 1), links it back, registers it
 * with func_8003E984 and sets bit 0 on success.
 *
 * @par Likely role
 * The distance culling/spawning of map-placed entities, sibling of the on-demand
 * cell spawner func_800402DC: cells are only materialised while a player is close,
 * and the two-player session keeps both views' spawn radii active.
 *
 * @par Unknown
 * What the D_800A7040 records hold beyond the position triple at +8, and what the
 * bit 4 flag of the +0x18 record marks.
 *
 * @par Callers
 * func_8003F570, and only when D_8009F30C is non-empty.
 */
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

    if (gPlayerCount >= 2) {
        func_80069580(1, (struct DestinationTriple *)&work.base2);
    }

    node = gNodeCList;
    while (node != 0) {
        entry = gSpawnRecordMap[node->arg08][node->type];
        next = node->next;

        if (entry != 0) {
            if (node->active != 0) {
                if (node->active->unk08 != node->id) {
                    if (prev != 0) {
                        prev->next = next;
                    } else {
                        gNodeCList = next;
                    }
                    node->next = 0;
                    gFreeBlock(node);
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

            if (gPlayerCount >= 2) {
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
