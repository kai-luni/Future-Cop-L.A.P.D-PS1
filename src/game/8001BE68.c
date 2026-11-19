// Categories: Graphics > Display Setup | Graphics > GPU
typedef unsigned char u8;
typedef short s16;
typedef int s32;
struct Display { u8 pad0[8]; s16 x, y; u8 pad1[10]; u8 flag; u8 rest[0x45]; };
struct Draw { u8 data[0x14]; };
extern struct Display gLayerBlocks[] __asm__("D_800A06E0");
extern struct Draw gDrawBlocks[] __asm__("D_800A06B0");
extern u8 gLayerIndex __asm__("D_8009F225");
extern u8 gDisplayEnabled __asm__("D_8009F608");
extern void func_80092910(int);
extern void func_80092BF4(int);
extern void func_80092A84(int);
extern void gSetDefDrawEnv(struct Display *, int, int, int, int) __asm__("func_80092140");
extern void gSetDefDispEnv(struct Draw *, int, int, int, int) __asm__("func_800921F4");
/**
 * @brief Resets the GPU library and installs the two 320x240 drawing areas.
 *
 * @return 0.
 *
 * @par Confirmed
 * func_80092910(0), func_80092BF4(0) and func_80092A84(0) reset the library state;
 * then func_80092140 fills D_800A06E0[0] at (0, 0) and [1] at (0, 0x100) with
 * 0x140 x 0xF0, func_800921F4 does the same for the draw blocks D_800A06B0, and
 * func_80092BF4(1) flushes.
 *
 * @par Likely role
 * The double-buffer set-up: 0x140 x 0xF0 is 320x240, and y = 0x100 puts the
 * second area in the other half of VRAM. Each entry's flag byte is cleared in the
 * same pass.
 *
 * @par Callers
 * The front-end/graphics initialisation.
 */
int func_8001BE68(void)
{
    int i;
    register int one __asm__("$3");
    register struct Display *p __asm__("$4");
    func_80092910(0);
    func_80092BF4(0);
    func_80092A84(0);
    gSetDefDrawEnv(&gLayerBlocks[0], 0, 0, 0x140, 0xF0);
    gSetDefDrawEnv(&gLayerBlocks[1], 0, 0x100, 0x140, 0xF0);
    gSetDefDispEnv(&gDrawBlocks[0], 0, 0, 0x140, 0xF0);
    gSetDefDispEnv(&gDrawBlocks[1], 0, 0x100, 0x140, 0xF0);
    gLayerBlocks[0].x = 0;
    gLayerBlocks[0].y = 0;
    gLayerBlocks[1].x = 0;
    gLayerBlocks[1].y = 0x100;
    i = 0;
    if (i < gLayerIndex) {
        one = 1;
        __asm__ volatile("" : "=r"(one) : "0"(one));
        p = gLayerBlocks;
        do {
            p->flag = one;
            p++;
            i++;
        } while (i < gLayerIndex);
    }
    func_80092BF4(1);
    gDisplayEnabled = 1;
    return 0;
}
