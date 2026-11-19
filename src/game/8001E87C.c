// Categories: Input > Controller Initialization
#include "game/types.h"

typedef struct {
    u8 a;
    u8 b;
    u8 c[2];
    s32 pad;
    s32 d[2];
    u8 pad2[6];
    u8 e;
    u8 f;
} Slot;

extern Slot gPadActuators[] __asm__("D_800A6FF8");
extern u8 gPadBuffers[] __asm__("D_800A78F0");

extern void func_8008BDB0(u8 *start, u8 *end);
extern void func_80089E00(void);
void gInitPadActuators(void) __asm__("func_8001E87C");


/**
 *  @brief Initializes two controller actuator slots and related state.
 *
 *  Each slot receives its port identifier, enabled flag and cleared motor
 *  and timer fields before the shared buffer and subsystem are initialized.
 *  @par Globals
 *  D_800A6FF8 contains the two slots; D_800A78F0 supplies a 0x24-byte range.
 *  @par Control flow
 *  An outer loop visits two slots; two inner loops clear paired fields.
 *  The two helper calls follow the loop.
 *  @par Calls
 *  func_8008BDB0 receives the buffer range; func_80089E00 performs setup.
 *  @par Side effects
 *  Writes slot fields and invokes helpers that may modify related state.
 *  @par Callers
 *  Confirmed: func_8001E4E4.
 *
 * @par Role and path
 * Initializes the two controller actuator slots in gPadActuators
 * (D_800A6FF8), clears the 0x24-byte gPadBuffers (D_800A78F0) range via
 * func_8008BDB0, and initializes the pad subsystem via func_80089E00. Called
 * by func_8001E4E4 during device initialization.
 */
void gInitPadActuators(void)
{
    s32 i;
    s32 j;
    Slot *p;
    u8 *q;
    s32 *r;


    for (i = 0; i < 2; i++) {
        p = &gPadActuators[i];
        p->a = i << 4;
        p->b = 0;
        p->e = 0;
        p->f = 1;
        j = 1;
        q = (u8 *)p + 1;
        for (; j >= 0; j--) {
            q[2] = 0;
            q--;
        }
        j = 1;
        r = (s32 *)((u8 *)p + 4);
        for (; j >= 0; j--) {
            r[2] = 0;
            r--;
        }

    }
    func_8008BDB0(gPadBuffers, gPadBuffers + 0x24);
    func_80089E00();
}
