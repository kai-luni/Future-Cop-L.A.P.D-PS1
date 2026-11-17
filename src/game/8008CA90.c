// Categories: Utilities > Memory Allocation
typedef unsigned int u32;
typedef struct Block { struct Block *next; int size; } Block;
extern Block *gSessionBlockBase __asm__("D_8009C7D0");

/**
 *  @brief Allocates an aligned payload from a circular free-block list.
 *
 *  The requested size is rounded up to eight bytes and includes a block header.
 *  A fitting block is removed whole or split from its high-address end.
 *  @param size Requested payload size in bytes.
 *  @return Pointer past the allocated header, or null for zero size or no fit.
 *  @par Globals
 *  D_8009C7D0 points to the free-list search position and is updated on success.
 *  @par Control flow
 *  Searches from the successor of the saved position through one circular pass.
 *  Exact fits are unlinked; larger blocks are shortened and split.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Changes free-list links or block sizes and writes the allocated block header.
 *  @par Callers
 *  Confirmed: func_8001E240.
 */
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
            gSessionBlockBase = prev;
            return p + 1;
        }
        if (p == gSessionBlockBase) return 0;
        prev = p;
        p = p->next;
    }
}
