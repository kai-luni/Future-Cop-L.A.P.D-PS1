// Categories: Graphics > Debug Display | Platform > Debug
#include "game/types.h"

struct PacketState {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 values[4];
    u8 flags[4];
};

struct Text8 {
    char b[8];
};

extern struct Text8 gTextBlock __asm__("D_8009F3B8");
extern u32 gExceptionCpuRegisters[] __asm__("D_800A6E58");
extern u32 gExceptionFrame[] __asm__("D_800A6E38");

extern void gResetIrqDispatch(void) __asm__("func_80091480");
extern void gSetVSyncCallback(void *) __asm__("func_80091510");
extern void gResetGraph(void *) __asm__("func_80092910");
extern void gClearImage(void *, int, int, int) __asm__("func_80092E10");
extern void *gSetDefDrawEnv(void *, int, int, int, int) __asm__("func_80092140");
extern void *gSetDefDispEnv(void *, int, int, int, int) __asm__("func_800921F4");
extern void gPutDispEnvCached(void *) __asm__("func_800934BC");
extern void gPutDrawEnv(void *) __asm__("func_800932F0");
extern void gSetDispMask(int) __asm__("func_80092BF4");
extern void gDrawDebugHexWord(int, int, unsigned int) __asm__("func_80077278");
__attribute__((noreturn)) void gDrawExceptionScreenAndHalt(void) __asm__("func_800773D8");


/**
 * @brief Endless debug screen: same set-up as func_8007757C, but it redraws
 *        forever.
 *
 * @return Never (declared `noreturn`).
 *
 * @par Confirmed
 * The 8-byte text block is copied from D_8009F3B8 with unaligned `lwl/lwr` pairs
 * written into the local frame (a hand-written carry-over of a `memcpy` the
 * compiler would not produce), the same display stack as func_8007757C runs, and
 * the function then loops over the two tables D_800A6E58/D_800A6E38 redrawing
 * until the machine is reset.
 *
 * @par Likely role
 * A development leftover: the "hang forever with diagnostics" screen. It is
 * reachable in the retail binary, so it is part of the shipped behaviour.
 */
__attribute__((noreturn)) void gDrawExceptionScreenAndHalt(void)
{
    u8 disp[0xB8];
    struct PacketState st[2];
    struct Text8 text;
    int i;
    int j;

    {
        register unsigned int hi __asm__("$2") = 0x800A0000;
        __asm__ volatile(
            "addiu $6,%0,%%lo(D_8009F3B8)\n\t"
            "lwl $3,3($6)\n\tlwr $3,0($6)\n\t"
            "lwl $4,7($6)\n\tlwr $4,4($6)\n\t"
            "swl $3,251($sp)\n\tswr $3,248($sp)\n\t"
            "swl $4,255($sp)\n\tswr $4,252($sp)"
            : : "r"(hi) : "$3", "$4", "$6", "memory");
    }

    gResetIrqDispatch();
    gSetVSyncCallback(0);
    gResetGraph(0);

    gClearImage(&text, 0, 0, 0);
    gSetDefDrawEnv(disp, 0, 0, 0x140, 0xF0);
    gSetDefDispEnv(st, 0, 0, 0x140, 0xF0);
    gPutDispEnvCached(st);
    gPutDrawEnv(disp);
    gSetDispMask(1);

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 8; i++) {
            gDrawDebugHexWord(j * 0x40 + 0x10, i * 0x10 + 0x10, gExceptionCpuRegisters[j * 8 + i]);
        }
    }

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 4; i++) {
            gDrawDebugHexWord(i * 0x40 + 0x10, j * 0x10 + 0xA0, gExceptionFrame[j * 4 + i]);
        }
    }

    for (;;) {
    }
}
