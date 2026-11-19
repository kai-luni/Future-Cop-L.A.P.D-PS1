// Categories: Game Engine > World Reset | Gameplay > Map | Resources > Memory Management
#include "game/types.h"

struct Unk31Node {
    s16 x;
    s16 unk02;
    s16 z;
    s16 unk06;
    s32 id;
    s16 extentX;
    s16 unk0E;
    s16 extentZ;
    s16 unk12;
    u8 flags;
    u8 unk15;
    s16 unk16;
    u8 unk18[0x0C];
    struct Unk31Node *next;
};

extern struct Unk31Node *gMapZoneList __asm__("D_8009F6F8");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeMapZoneList(void) __asm__("func_80031990");


/**
 * @brief Frees all nodes in the map trigger/zone linked list D_8009F6F8 and clears its head.
 *
 * Walks the singly-linked list headed by D_8009F6F8, capturing node->next (+0x24) before
 * handing each node to func_800181D4, and resets D_8009F6F8 to 0 after the walk completes.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F6F8 - head pointer of the map trigger/zone linked list (read, drained, cleared to 0).
 *
 * @par Control flow
 * Single while-loop over node != 0 saving node->next before freeing each node.
 *
 * @par Calls
 * func_800181D4 - frees each Unk31Node back to its pool or allocator.
 *
 * @par Side effects
 * Deallocates every trigger/zone node in D_8009F6F8 and sets D_8009F6F8 = 0.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D90, during between-mission global subsystem reset) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Frees every zone node of gMapZoneList (D_8009F6F8) with gFreeBlock
 * (func_800181D4), reading next at +0x24 before each free, then clears the
 * head. Its only caller is gResetMissionSubsystems (func_80024D14), which
 * also runs gFreeCameraList, so it is the teardown counterpart of the zone
 * builder func_800319D4; func_80031984 only zeroes the head at mission boot
 * without freeing. The port can treat the zone list as owned by the mission
 * and dropped as a whole.
 */
void gFreeMapZoneList(void)
{
    struct Unk31Node *node;
    struct Unk31Node *next;

    node = gMapZoneList;
    while (node != 0) {
        next = node->next;
        gFreeBlock(node);
        node = next;
    }
    gMapZoneList = 0;
}
