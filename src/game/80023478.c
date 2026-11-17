// Categories: Graphics > Sprite/Asset Subsystem | Resources > Teardown
#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern u8 D_8009F26C;
extern s32 D_8009F278;
extern s32 D_8009F27C;
extern u8 D_80023314[];
extern Unk800A0A00 gObjectSlots[8] __asm__("D_800A0A00");

extern void func_80098708(s32 arg0);
extern s32 func_8001F12C(s32 callback);
extern void func_80021250(void);
extern void func_80023BF8(void);
extern void func_80021C80(void);
extern void func_80044FEC(void);

/**
 * @brief Tears the sprite/asset subsystem down when it is running.
 *
 * The whole body is guarded by D_8009F26C ("running", set by func_8002333C):
 * the two work buffers D_8009F27C and D_8009F278 are released with
 * func_80098708() and cleared, then the callback and the resource groups are
 * removed (func_8001F12C(D_80023314), func_80021250(), func_80023BF8(),
 * func_80021C80(), func_80044FEC()) and D_8009F26C = 0.
 *
 * @return Nothing. Calling it when the subsystem is not running does nothing,
 *         which is why func_8002333C can call it unconditionally at start-up.
 *
 * @par Globals
 * D_8009F26C (running flag), D_8009F278/D_8009F27C (buffers),
 * D_800A0A00 (eight object slots, not cleared here), D_80023314 (callback
 * pointer).
 *
 * @par Callers
 * func_8002333C and func_80076FE0, plus the unit at 0x80023860.
 */
void func_80023478(void)
{
    if (D_8009F26C != 0) {
        if (D_8009F27C != 0) {
            func_80098708(D_8009F27C);
            D_8009F27C = 0;
        }
        if (D_8009F278 != 0) {
            func_80098708(D_8009F278);
            D_8009F278 = 0;
        }
        func_8001F12C((s32)D_80023314);
        func_80021250();
        func_80023BF8();
        func_80021C80();
        func_80044FEC();
        D_8009F26C = 0;
    }
}
