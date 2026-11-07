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
