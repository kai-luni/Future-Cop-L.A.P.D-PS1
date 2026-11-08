typedef unsigned int u32;
typedef int s32;
struct Entry { u32 addr; u32 size; };
extern s32 D_8009EF08;
extern struct Entry *D_8009EF0C;
void func_80097DF8(void)
{
    s32 i, j;
    u32 a, b, size;
    {
        register s32 limit __asm__("$13");
        register struct Entry *base __asm__("$8");
        register struct Entry *cur __asm__("$7");
        register u32 deleted __asm__("$10");
        register u32 mask __asm__("$11");
        register u32 flag __asm__("$12");
        register s32 initial __asm__("$2");
        initial = D_8009EF08;
        i = 0;
        if (initial >= 0) {
            flag = 0x80000000; deleted = 0x2fffffff; mask = 0x0fffffff;
            base = D_8009EF0C; limit = initial; cur = base;
            do {
                if (cur->addr & flag) {
                    register struct Entry *scan __asm__("$3");
                    struct Entry *next;
                    register u32 offset __asm__("$2");
                    j = i + 1;
                    offset = j << 3;
                    __asm__("" : "+r" (offset));
                    scan = (struct Entry *)(offset + (u32)base);
                again:
                    if (scan->addr == deleted) { scan++; j++; goto again; }
                    __asm__("" : "+r" (j));
                    next = (struct Entry *)((j << 3) + (u32)base);
                    if ((next->addr & flag) && (next->addr & mask) == (cur->addr & mask) + cur->size) {
                        next->addr = deleted;
                        cur->size += next->size;
                        continue;
                    }
                }
                cur++; i++;
            } while (i <= limit);
        }
    }
    {
        register s32 count __asm__("$2");
        register s32 count2 __asm__("$4");
        register u32 deleted __asm__("$5");
        count = D_8009EF08;
        __asm__ volatile (".set reorder\n.set noreorder" : "+r" (count));
        i = 0;
        if (count >= 0) {
            deleted = 0x2fffffff;
            __asm__("" : "+r" (deleted));
            count2 = count;
            do {
                __asm__ volatile (".set reorder\n.set noreorder");
                if (D_8009EF0C[i].size == 0) D_8009EF0C[i].addr = deleted;
                i++;
            } while (i <= count2);
        }
    }
    for (i = 0; i <= D_8009EF08; i++) {
        if (D_8009EF0C[i].addr & 0x40000000) break;
        for (j = i + 1; j <= D_8009EF08; j++) {
            if (D_8009EF0C[j].addr & 0x40000000) break;
            if ((D_8009EF0C[j].addr & 0x0fffffff) < (D_8009EF0C[i].addr & 0x0fffffff)) {
                a = D_8009EF0C[i].addr;
                b = D_8009EF0C[i].size;
                D_8009EF0C[i].addr = D_8009EF0C[j].addr;
                D_8009EF0C[i].size = D_8009EF0C[j].size;
                D_8009EF0C[j].addr = a;
                D_8009EF0C[j].size = b;
            }
        }
    }
    for (i = 0; i <= D_8009EF08; i++) {
        if (D_8009EF0C[i].addr & 0x40000000) break;
        if (D_8009EF0C[i].addr == 0x2fffffff) {
            D_8009EF0C[i].addr = D_8009EF0C[D_8009EF08].addr;
            D_8009EF0C[i].size = D_8009EF0C[D_8009EF08].size;
            D_8009EF08 = i;
            break;
        }
    }
    for (i = D_8009EF08 - 1; i >= 0; i--) {
        if (!(D_8009EF0C[i].addr & 0x80000000)) break;
        D_8009EF0C[i].addr = (D_8009EF0C[i].addr & 0x0fffffff) | 0x40000000;
        size = D_8009EF0C[D_8009EF08].size;
        D_8009EF08 = i;
        D_8009EF0C[i].size += size;
    }
}
