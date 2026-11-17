// Categories: Graphics > Renderer > Map Cell Geometry | Graphics > HUD > Minimap
#include "game/types.h"

typedef struct Node {
    s32 unk0;
    s32 unk4;
    u8 unk8[0xA];
    s16 unk12;
    u8 unk14[0x2C];
    struct Node *next;
} Node;

typedef struct MaskEntry {
    s32 mask;
    u8 pad[12];
} MaskEntry;

extern Node **D_8009F780;
extern s32 D_8009F78C;
extern s32 D_8009F794;
extern s32 gDiscShadeByte __asm__("D_8009FAA4");

extern void func_800497A8(void);
extern void func_8004A048(Node *node, s32 arg1, s32 arg2);

/**
 * @brief Draws the map-cell objects in a 5x5 cell window, filtered by a
 *        per-cell mask table.
 *
 * @param arg0, arg1 The window's origin cell.
 * @param arg2 25 mask entries (one per window cell, stride 0x10).
 * @return Nothing.
 *
 * @par Confirmed
 * Returns without a grid (D_8009F780 == 0). When D_8009F794 + 0x4B0 <
 * D_8009FAA4 the grid is rebuilt via func_800497A8 and the stamp updated.
 * For each of the 25 cells with a non-zero mask, walks the node list at
 * D_8009F780[(arg0 + i) + D_8009F78C * (arg1 + j)]; a node is drawn via
 * func_8004A048(node, col, row) when its unk4 is outside [0, D_8009FAA4]
 * and the mask shares a bit with the node's unk12.
 *
 * @par Likely role
 * The minimap/region draw pass of func_80066E98: the mask table selects
 * which object classes each cell shows, the unk4 range test skips nodes
 * inside the current zoom window.
 *
 * @par Unknown
 * The unit of the 0x4B0 rebuild threshold and the mask bit assignment.
 *
 * @par Callers
 * func_80066E98.
 */
void func_8004A924(s32 arg0, s32 arg1, MaskEntry *arg2)
{
    s32 i;
    s32 j;
    s32 mask;
    Node *node;
    MaskEntry *entry;

    if (D_8009F780 == 0) {
        return;
    }

    if (D_8009F794 + 0x4B0 < gDiscShadeByte) {
        func_800497A8();
        D_8009F794 = gDiscShadeByte;
    }

    entry = arg2;
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 5; i++) {
            mask = entry->mask;
            entry++;
            if (mask == 0) {
                continue;
            }
            node = D_8009F780[(arg0 + i) + D_8009F78C * (arg1 + j)];
            if (node == 0) {
                continue;
            }
            do {
                if (node->unk4 < 0 || node->unk4 > gDiscShadeByte) {
                    if (mask & node->unk12) {
                        func_8004A048(node, arg0 + i, arg1 + j);
                    }
                }
                node = node->next;
            } while (node != 0);
        }
    }
}
