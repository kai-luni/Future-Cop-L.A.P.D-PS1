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
extern Header *D_8009C7D0;
extern Snapshot D_800A6568;
extern Snapshot D_800A8F80;
extern Header *D_800A8E78;

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
 */
void func_8008CB40(Header *ptr)
{
    Header *p;
    Header *bp;
    register Snapshot *s __asm__("$4");
    register Snapshot *t __asm__("$3");
    register Header *next __asm__("$2");
    register char *base __asm__("$1");
    bp = ptr - 1;
    p = D_8009C7D0;
    for (; !(bp > p && bp < p->next); p = p->next) {
        if (p >= p->next && (bp > p || bp < p->next))
            break;
    }
    __asm__ volatile("" : : : "memory");
    s = &D_800A6568;
    __asm__ volatile("" : "+r"(s));
    s->self = bp;
    s->size = bp->size;
    __asm__ volatile("" : : : "memory");
    next = bp->next;
    __asm__ volatile("" : "+r"(next));
    t = &D_800A8F80;
    __asm__ volatile("" : "+r"(t));
    t->self = p;
    s->next = next;
    t->size = p->size;
    t->next = p->next;
    D_800A8E78 = D_8009C7D0;
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
