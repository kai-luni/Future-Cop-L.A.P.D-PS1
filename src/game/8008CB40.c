// Categories: Utilities > Memory
typedef struct Header {
    struct Header *next;
    unsigned int size;
} Header;
typedef struct Snapshot {
    Header *self;
    Header *next;
    unsigned int size;
} Snapshot;
extern Header *gSessionBlockBase __asm__("D_8009C7D0");
extern Snapshot gFreedBlockSnap __asm__("D_800A6568");
extern Snapshot gPrevBlockSnap __asm__("D_800A8F80");
extern Header *gSavedFreeCursor __asm__("D_800A8E78");
void gFreeHeapBlock(Header *ptr) __asm__("func_8008CB40");


/**
 *  @brief Returns a block to the circular free list and coalesces adjacent blocks.
 *
 *  The header immediately before the supplied pointer is inserted in address
 *  order, with snapshots recorded before the list is modified.
 *  @param ptr Pointer immediately following the block header.
 *  @par Globals
 *  D_8009C7D0 is the free-list cursor; D_800A8E78 saves its previous value.
 *  D_800A6568 and D_800A8F80 receive snapshots of the block and predecessor.
 *  @par Control flow
 *  Searches through the circular list, handling its address wrap, then merges
 *  with the successor if adjacent and nonempty and with the predecessor if adjacent.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates snapshots, free-list headers, and D_8009C7D0 to the predecessor.
 *  @par Callers
 *  Confirmed: func_8001E2C0.
 *
 * @par Role and path
 * Inserts the block header preceding ptr into the circular free list at
 * gSessionBlockBase (D_8009C7D0) in address order, coalescing with adjacent
 * nonempty successor and predecessor blocks. Records pre-merge snapshots in
 * D_800A6568 and D_800A8F80, saves the prior cursor in D_800A8E78, and
 * updates gSessionBlockBase via 0x800A0000 - 0x3830. Called by func_8001E2C0.
 */
void gFreeHeapBlock(Header *ptr)
{
    Header *p;
    Header *bp;
    register Snapshot *s __asm__("$4");
    register Snapshot *t __asm__("$3");
    register Header *next __asm__("$2");
    register char *base __asm__("$1");
    bp = ptr - 1;
    p = gSessionBlockBase;
    for (; !(bp > p && bp < p->next); p = p->next) {
        if (p >= p->next && (bp > p || bp < p->next))
            break;
    }
    __asm__ volatile("" : : : "memory");
    s = &gFreedBlockSnap;
    __asm__ volatile("" : "+r"(s));
    s->self = bp;
    s->size = bp->size;
    __asm__ volatile("" : : : "memory");
    next = bp->next;
    __asm__ volatile("" : "+r"(next));
    t = &gPrevBlockSnap;
    __asm__ volatile("" : "+r"(t));
    t->self = p;
    s->next = next;
    t->size = p->size;
    t->next = p->next;
    gSavedFreeCursor = gSessionBlockBase;
    if (bp + bp->size == p->next && p->next->size != 0) {
        bp->size += p->next->size;
        bp->next = p->next->next;
    } else {
        bp->next = p->next;
    }
    if (p + p->size == bp) {
        p->size += bp->size;
        p->next = bp->next;
    } else {
        p->next = bp;
    }
    base = (char *)0x800A0000;
    __asm__ volatile("" : "+r"(base));
    *(Header **)(base - 0x3830) = p;
}
