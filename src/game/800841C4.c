// Categories: Game Engine > Node Management
#include "game/types.h"

struct Node841C4 {
    u8 pad00[0x4C];
    u8 flags;
    u8 timer;
    u8 pad4E[2];
    s32 field50;
    u16 id;
    u8 pad56[2];
    struct Node841C4 *next;
};

struct Cell841C4 {
    struct Node841C4 *head;
    u32 unk04;
};

struct LinkState;
extern struct LinkState *gMissionState __asm__("D_8009F934");
extern struct Cell841C4 *gBucketArray __asm__("D_8009F938");
extern s32 gMissionGridWidth __asm__("D_8009F93C");
extern s32 gMissionGridHeight __asm__("D_8009F940");
extern s32 gDiscShadeByte[] __asm__("D_8009FAA4");
extern void func_800195D0(struct LinkState *state, int *link);

/**
 *  @brief Finds the first node with a matching ID and applies a command.
 *
 *  Cells are searched in descending index order, with each cell's linked list
 *  traversed from its head. Commands remove a node or update its fields and flags.
 *  @param id ID to match against each node's 16-bit ID.
 *  @param mode Command value: 0x19, 0x1A, or 0x1B.
 *  @param index Command-dependent value controlling removal, timing, or flags.
 *  @par Globals
 *  D_8009F938 holds cells; D_8009F93C and D_8009F940 determine their count.
 *  D_8009F934 is passed for node removal; D_8009FAA4 supplies a field base.
 *  @par Control flow
 *  Stops at the first matching node, including for an unrecognized command.
 *  Command branches distinguish zero, negative, and positive index values as applicable.
 *  @par Calls
 *  func_800195D0 handles a node unlinked by command 0x19 with index zero.
 *  @par Side effects
 *  May unlink a node or modify its field, timer, and flags; otherwise does nothing.
 *  @par Callers
 *  Confirmed: func_80069D68.
 */
void func_800841C4(s32 id, s32 mode, s32 index)
{
    register s32 command __asm__("$11") = mode;
    s32 i;
    struct Node841C4 *node;
    struct Node841C4 *prev;

    for (i = gMissionGridWidth * gMissionGridHeight - 1; (u32)i < 0x80000000U; i--) {
        node = gBucketArray[i].head;
        prev = 0;
        if (node == 0) {
            continue;
        }

        while (node != 0) {
            if (node->id == id) {
                switch (command) {
                case 0x19:
                    if (index == 0) {
                        if (prev == 0) {
                            gBucketArray[i].head = node->next;
                        } else {
                            prev->next = node->next;
                        }
                        func_800195D0(gMissionState, (int *)node);
                    } else {
                        if (index < 0) { node->field50 = -1; }
                        else { node->field50 = gDiscShadeByte[0] + index * 300; }
                        node->flags &= 0x7F;
                    }
                    return;

                case 0x1A:
                    if (index == 0) {
                        node->flags |= 0x08;
                    } else {
                        node->timer = 0x12C / index;
                        node->flags &= 0xF7;
                    }
                    return;

                case 0x1B:
                    if (index == 0) {
                        node->flags |= 0x10;
                    } else {
                        node->flags &= 0xEF;
                    }
                    return;
                }
                return;
            }

            prev = node;
            node = node->next;
        }
    }
}
