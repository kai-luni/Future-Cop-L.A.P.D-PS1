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
extern void gResetGraph(int) __asm__("func_80092910");
extern void func_80092BF4(int);
extern void func_80092A84(int);
extern void gSetDefDrawEnv(struct Display *, int, int, int, int) __asm__("func_80092140");
extern void gSetDefDispEnv(struct Draw *, int, int, int, int) __asm__("func_800921F4");

int gInitDoubleBufferedGpuEnvs(void) __asm__("func_8001BE68");

/**
 * @brief Resets GPU state and initializes two 320-by-240 draw and display
 * environments.
 * @return Always zero.
 *
 * @par Confirmed mechanics
 * Calls three GPU helpers with zero. Builds draw and display environments at
 * VRAM Y offsets 0 and 0x100, writes draw offsets to match, and sets byte
 * +0x16 of each configured draw environment. Calls the display-mask helper
 * with one and sets the enable byte.
 *
 * @par Globals
 * Writes gLayerBlocks, gDrawBlocks, and gDisplayEnabled. Reads gLayerIndex as
 * the loop bound.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitFadeLayers.
 *
 * @par Role and path
 * Graphics startup prepares the paired drawing and display regions before
 * rendering.
 *
 * @par Unknown
 * The routine takes the count of draw environment flags from an existing
 * global byte.
 */
int gInitDoubleBufferedGpuEnvs(void)
{
    int i;
    register int one __asm__("$3");
    register struct Display *p __asm__("$4");
    gResetGraph(0);
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
