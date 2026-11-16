// Categories: Physics > Force Fields | Game Engine > Object Setup
#include "game/types.h"

typedef struct {
    u8 pad00[4];
    s32 unk04;
    s32 unk08;
} Unk39E8C_B;

typedef struct {
    u8 pad00[0x18];
    s32 unk18;
} Unk39E8C_C;

typedef struct {
    u8 pad00[4];
    Unk39E8C_C *unk04;
    u8 *unk08;
} Unk39E8C_A;

typedef struct {
    u8 pad00[0x21];
    u8 unk21;
    u8 pad22[2];
    u32 unk24;
    u8 pad28[0x28];
    s32 unk50;
    s32 unk54;
    u8 pad58[8];
    s32 unk60;
    u8 pad64[0xB8];
    s32 unk11C[6];
    s16 unk134;
    s16 unk136;
    s16 unk138;
    s16 unk13A;
    u8 unk13C;
    u8 unk13D;
    u16 unk13E;
    u8 unk140;
    u8 unk141;
    u8 pad142[2];
    s32 unk144;
} Unk39E8C_O;

typedef struct {
    u8 pad00[0x24];
    u8 unk24[4];
    s16 unk28;
    u8 pad2A[0xE];
    u16 unk38;
    s16 unk3A;
    s16 unk3C;
    s16 unk3E;
    u8 unk40;
    u8 unk41;
    u8 unk42;
} Unk39E8C_I;

extern s32 func_8002AD58(s32, Unk39E8C_O *, Unk39E8C_A *);
extern s32 func_80067684(void *, s32);
extern void func_80050E0C(void *, void *, s32, void *, void *);

/**
 * @brief Spawn-time initializer of the force-field entity class.
 *
 * Runs the shared base handler, then places the field on the topmost terrain
 * surface, installs its motion/path block and fills the parameter block at
 * +0x134..+0x144 (range, strength, on/off phase times, curve/mode bytes,
 * effect flags, force multiplier) that the tick handler func_8003A160 reads.
 *
 * @param arg0 Message number, forwarded to the base handler (spawn is 2).
 * @param obj Force-field object block (0x148 bytes for this class).
 * @param arg2 Spawn payload: +0x04 definition record (+0x18 selector),
 *        +0x08 input definition with the field parameters.
 *
 * @par Globals
 * None touched directly; all state lives in the object block.
 *
 * @par Control flow
 * Straight-line setup. One branch: when the definition selector is 0 the
 * height is lifted by input->unk28 * 16 above the terrain. The effect flag
 * byte gets 0xC0 when both phase times are non-zero (periodic blinking),
 * else 0x80. A small loop clears the two force vectors at +0x11C.
 *
 * @par Calls
 * func_8002AD58 - base entity handler, runs the default spawn path first.
 * func_80067684 - terrain surface selector, mode -1 returns the top surface.
 * func_80050E0C - installs the motion/path data at obj+0xA0 from the input.
 *
 * @par Side effects
 * Writes obj +0x21 (cleared, old value saved to +0x141), +0x24 bit31 cleared,
 * +0x54/+0x60 height, the +0xA0 motion block and +0x11C..+0x144 parameters;
 * +0x144 is 0x10000000 / range, the per-tick force multiplier.
 *
 * @par Callers
 * Confirmed: func_8003A844 (message 2 of the class handler, size query 0x148).
 */
void func_80039E8C(s32 arg0, Unk39E8C_O *obj, Unk39E8C_A *arg2)
{
    Unk39E8C_I *in;
    s32 i;
    u8 t;
    register s32 h __asm__("$4");
    u8 x;

    func_8002AD58(arg0, obj, arg2);
    in = (Unk39E8C_I *)arg2->unk08;
    obj->unk54 = obj->unk60 = func_80067684(&obj->unk50, -1);
    func_80050E0C(&in->unk24, (u8 *)obj + 0xA0, arg2->unk04->unk18, obj, &obj->unk50);
    obj->unk24 &= 0x7FFFFFFF;
    if (arg2->unk04->unk18 == 0) {
        obj->unk54 = func_80067684(&obj->unk50, -1) + in->unk28 * 16;
    }
    obj->unk134 = in->unk38;
    obj->unk136 = (in->unk3A << 8) / 300;
    obj->unk138 = (in->unk3C * 300) / 256;
    obj->unk13A = (in->unk3E * 300) / 256;
    obj->unk13C = in->unk40;
    h = obj->unk138;
    obj->unk13D = in->unk41;
    t = in->unk42;
    obj->unk140 = t;
    obj->unk13E = obj->unk138;
    if (h != 0 && obj->unk13A != 0) {
        x = t;
        __asm__("" : "+r"(x));
        x |= 0xC0;
    } else {
        x = obj->unk140 | 0x80;
    }
    obj->unk140 = x;
    for (i = 0; i < 2; i++) {
        obj->unk11C[i * 3] = 0;
        obj->unk11C[i * 3 + 1] = 0;
        obj->unk11C[i * 3 + 2] = 0;
    }
    obj->unk141 = obj->unk21;
    obj->unk21 = 0;
    obj->unk144 = 0x10000000 / obj->unk134;
}
