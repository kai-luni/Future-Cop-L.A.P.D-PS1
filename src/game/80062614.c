// Categories: Physics > Collision > Broadphase Grid Pairs | Game Engine > Frame
typedef unsigned char u8;
typedef struct Node { void *object; struct Node *next; } Node;
typedef struct { int x, y, width, height; } Grid;
extern Grid gCellClipRect __asm__("D_800A2970");
extern Node gCellNodeArray[] __asm__("D_800A2990");
extern u8 *gCellMap __asm__("D_8009F820");
extern void gProcessCellCollisionPairs(Node *) __asm__("func_800623FC");
extern void gProcessAdjacentCellCollisionPairs(Node *, Node *) __asm__("func_80062504");
void gSweepCollisionGridPairs(void) __asm__("func_80062614");

/**
 * @brief Broadphase pass over the marked viewport grid: tests the objects inside
 *        every cell and against the objects of the neighbouring cells.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * The function takes no arguments; everything is read from globals or from the
 * objects they point at.
 * The pass works on the rectangle D_800A2970 (x, y, width, height) written by the
 * caller, the cell map D_8009F820 (one byte per cell, 0 = empty) and the node
 * array D_800A2990, indexed by that byte.
 * For every row except the last it handles the first cell, the cells in between
 * and the last cell: after func_800623FC(&base[index]) - the pairs inside one
 * cell - it calls func_80062504(&base[index], &base[value]) for the cell to the
 * right (cur[1]), the two cells below (next[0], next[1]) and, in the interior
 * only, the cell below-left (next[-1]). The last row only pairs the cell with its
 * right neighbour, and the very last cell gets func_800623FC alone. A cell byte
 * of 0 is skipped everywhere.
 *
 * @par Likely role
 * The broadphase driver of the object-versus-object collision system: the
 * right/down/down-right/down-left neighbourhood is exactly what makes every pair
 * of adjacent cells meet once, so the function turns the marked viewport
 * rectangle into the list of object pairs to test. func_80062FB4 calls it last,
 * after it has cleared and filled the cell map for the current view.
 *
 * @par Unknown
 * What writes the cell bytes of D_8009F820 (func_80062FB4 only clears the reused
 * region; the per-view passes func_80062C3C and func_80062874 fill it) and why
 * node index 0 is skipped - it is probably the "no object" sentinel.
 *
 * @par Callers
 * func_80062FB4 only.
 */
void gSweepCollisionGridPairs(void)
{
    register Node *node __asm__("$16");
    register u8 *next __asm__("$17");
    register u8 *cur __asm__("$18");
    register int cols __asm__("$19");
    Node *base;
    register int rows __asm__("$21");
    register int width __asm__("$22");
    register int value __asm__("$5");
    register int index __asm__("$2");
    register int height __asm__("$3");
    height = gCellClipRect.height;
    cur = gCellMap;
    width = gCellClipRect.width;
    __asm__("" : "=r"(height) : "0"(height), "r"(cur), "r"(width));
    rows = height - 1;
    __asm__ volatile ("" : : : "memory");
    __asm__("" : "=r"(index));
    base = gCellNodeArray;
    if (rows > 0) {
        __asm__("" : : "r"(index));
        next = cur + width;
        do {
            index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                node = &base[index];
                gProcessCellCollisionPairs(node);
                if ((value = cur[1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                if ((value = next[0])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                if ((value = next[1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
            }
            next++; cur++;
            cols = width - 2;
            if (cols > 0) {
                do {
                    index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                        node = &base[index];
                        gProcessCellCollisionPairs(node);
                        if ((value = cur[1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                        if ((value = next[0])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                        if ((value = next[1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                        if ((value = next[-1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                    }
                    next++; cur++; cols--;
                } while (cols > 0);
            }
            index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                node = &base[index];
                gProcessCellCollisionPairs(node);
                if ((value = next[0])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
                if ((value = next[-1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
            }
            next++; cur++; rows--;
        } while (rows > 0);
    }
    cols = width - 1;
    if (cols > 0) {
        do {
            index = *cur;
            __asm__("" : "=r"(index) : "0"(index));
            if (index) {
                node = &base[index];
                gProcessCellCollisionPairs(node);
                if ((value = cur[1])) gProcessAdjacentCellCollisionPairs(node, &base[value]);
            }
            cur++; cols--;
        } while (cols > 0);
    }
    if ((index = *cur)) gProcessCellCollisionPairs(&base[index]);
}
