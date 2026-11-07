typedef unsigned int u32;
typedef struct Block { struct Block *next; int size; } Block;
extern Block *D_8009C7D0;

void *func_8008CA90(u32 size)
{
    Block *p, *prev;
    int n;
    if (!size) return 0;
    while (size & 7) size++;
    __asm__ volatile("addiu $2,%3,7\n\tsrl $2,$2,3\n\tlui %0,%%hi(D_8009C7D0)\n\tlw %0,%%lo(D_8009C7D0)(%0)\n\taddiu %1,$2,1\n\tlw %2,0(%0)\n\tnop" : "=&r"(prev), "=&r"(n), "=r"(p) : "r"(size) : "$2");
    for (;;) {
        if (p->size >= n) {
            if (p->size == n) {
                prev->next = p->next;
            } else {
                p->size -= n;
                p += p->size;
                p->size = n;
            }
            D_8009C7D0 = prev;
            return p + 1;
        }
        if (p == D_8009C7D0) return 0;
        prev = p;
        p = p->next;
    }
}
