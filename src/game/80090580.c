#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

typedef struct {
    struct CdlLOC loc;
    u32 size;
    u8 name[16];
} DirEntry;

extern s32 D_8009CBC0;
extern s32 D_8009C8B0;
extern u8 D_800A445C[];
extern u8 D_800A5A80[];
extern DirEntry D_800A3E80[];
extern u16 D_80012754;
extern s16 D_80012758;
extern s8 D_8001275A;
extern u8 D_80012718[];
extern u8 D_80012738[];
extern u8 D_8001275C[];
extern u8 D_80012778[];

extern struct CdlLOC *func_8008E61C(s32 sector, struct CdlLOC *location);
extern s32 func_8009081C(s32 sectors, s32 sector, u8 *buf);
extern void func_8008E000();
extern void func_8008CD10(u8 *dst, u8 *src, u32 n);
extern void *memcpy(void *, const void *, u32);

#define PRINT_STR(S) __asm__ volatile(".set\tnoreorder\n\tlui $4,%%hi(" #S ")\n\tjal func_8008E000\n\taddiu $4,$4,%%lo(" #S ")\n\t.set\treorder" \
    : : : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory")

#define LOADV(op, S, o) ({ s32 v_; __asm__ volatile("lui %0,%%hi(" #S ")\n\taddu %0,%0,%1\n\t" op " %0,%%lo(" #S ")(%0)" : "=&r"(v_) : "r"(o)); v_; })
#define HIADDR(o) ({ u8 *a_; __asm__ volatile("lui %0,0x800a\n\taddu %0,%0,%1" : "=&r"(a_) : "r"(o)); a_; })
#define LOADX(op, S, o) ({ s32 v_; __asm__("lui %0,%%hi(" #S ")\n\taddu %0,%0,%1\n\t" op " %0,%%lo(" #S ")(%0)" : "=&r"(v_) : "r"(o)); v_; })

s32 func_80090580(s32 arg0)
{
    u8 *p;
    u8 *end;
    s32 i;
    s32 sector;
    u8 *h;

    if (arg0 == D_8009CBC0) {
        return 1;
    }
    if (func_8009081C(1, LOADX("lw", D_800A445C, arg0 * 0x2C), D_800A5A80) != 1) {
        if (D_8009C8B0 > 0) {
            PRINT_STR(D_80012718);
        }
        return -1;
    }
    if (D_8009C8B0 >= 2) {
        PRINT_STR(D_80012738);
    }
    p = D_800A5A80;

    i = 0;
    while (p < D_800A5A80 + 0x800) {
        if (*p == 0) {
            break;
        }
        __asm__("" : : "r"(&D_800A3E80[i]));
        memcpy(&sector, p + 2, 4);
        func_8008E61C(sector, &D_800A3E80[i].loc);
        memcpy(&D_800A3E80[i].size, p + 10, 4);
        switch (i) {
        case 0:
            *(u16 *)D_800A3E80[0].name = D_80012754;
            break;
        case 1:
            {
                s32 a = D_80012758;
                s32 b = D_8001275A;
                __asm__("" : "=r"(a), "=r"(b) : "0"(a), "1"(b));
                *(s16 *)D_800A3E80[1].name = a;
                D_800A3E80[1].name[2] = b;
            }
            break;
        default:
            func_8008CD10(D_800A3E80[i].name, p + 0x21, p[0x20]);
            D_800A3E80[i].name[p[0x20]] = 0;
            break;
        }
        if (D_8009C8B0 >= 2) {
            s32 off = i * sizeof(DirEntry);
            __asm__("" : : "r"(i), "r"(off));
            func_8008E000(D_8001275C, LOADV("lbu", D_800A3E80, off), LOADV("lbu", D_800A3E81, off),
                          LOADV("lbu", D_800A3E82, off), *(u32 *)(HIADDR(off) + 0x3E84), D_800A3E80[i].name);
        }
        p += *p;
        i++;
        if (i >= 0x40) {
            break;
        }
    }
    D_8009CBC0 = arg0;
    if (i < 0x40) {
        D_800A3E80[i].name[0] = 0;
    }
    if (D_8009C8B0 >= 2) {
        func_8008E000(D_80012778, i);
        __asm__ volatile(".set\tnoreorder\n\t.set reorder");
    }
    return 1;
}
