typedef unsigned int u32;
typedef unsigned char u8;
typedef struct { char pad[0x5c]; int mode; } Context;
typedef struct { char pad[0x18]; int count; } Other;
typedef struct { u32 x,y,w,h; int width,height; } Rect;
typedef struct { u8 flag; char pad[0x43]; } Entry;
extern volatile Context *D_8009FA94;
extern Other *D_8009F80C;
extern int D_8009FA0C;
extern volatile int D_8009F82C, D_8009F828;
extern u8 D_8009F834, D_8009F835, D_8009F836, D_8009F837;
extern u32 *D_8009F820;
extern Rect D_800A2970;
extern Entry D_800A8DC0[];
extern void func_80062C3C(void), func_80062874(void), func_80062614(void);
void func_80062FB4(void)
{
    u32 x, y, x2, y2, w, h, n, i;
    u32 *p;
    Entry *entry;
    D_8009F82C = 0;
    D_8009F828 = 0;
    if (D_8009FA94->mode == 1) {
        h = D_800A2970.height - 16;
        w = D_800A2970.width - D_8009F80C->count * 8 + 8;
        x = y = (D_8009F80C->count - 1) * 4;
    } else if (D_8009FA0C == 1) {
        h = w = 20;
        x = D_8009F834 * 4;
        y = D_8009F835 * 4;
    } else {
        x = D_8009F834 * 4;
        y = D_8009F835 * 4;
        x2 = D_8009F836 * 4;
        y2 = D_8009F837 * 4;
        if (x2 < x) { x2 ^= x; x ^= x2; x2 ^= x; }
        if (y2 < y) { y2 ^= y; y ^= y2; y2 ^= y; }
        w = x2 - x + 20;
        h = y2 - y + 20;
    }
    n = w * h;
    p = D_8009F820;
    D_800A2970.x = x;
    D_800A2970.y = y;
    D_800A2970.w = w;
    D_800A2970.h = h;
    { register u32 count __asm__("$3");
    count = n >> 2;
    while (count) { *p++ = 0; count--; } }
    if (D_8009FA0C == 1) func_80062C3C();
    else func_80062874();
    i = 0;
    __asm__ volatile ("" : "+r" (i));
    entry = D_800A8DC0;
    do { entry->flag = 0; i++; entry++; } while (i < 2);
    func_80062614();
}
