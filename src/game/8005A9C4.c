// Categories: Graphics > HUD | Gameplay > Radar | Graphics > 2D Drawing > Lines
#include "game/types.h"

struct Node5A9C4 {
    u8 unk00[0xA4];
    u32 unkA4;
    u8 flags[8];
    struct {
        s16 a;
        s16 b;
    } pos[8];
};

struct Unk5A9C4AC {
    u8 unk00[0xC1];
    s8 unkC1;
    u8 unkC2[0x300 - 0xC2];
    struct Node5A9C4 *unk300;
};

struct Unk5A9C4 {
    u8 unk00[0xAC];
    struct Unk5A9C4AC *unkAC;
    s16 unkB0;
    s16 unkB2;
};

struct Unk5A9C4Other {
    u8 unk00[0x50];
    s32 unk50;
    u8 unk54[4];
    s32 unk58;
};

struct Unk5A9C4FA94 {
    u8 unk00[0x5C];
    s32 unk5C;
};

extern s32 gPlayerCount __asm__("D_8009FA0C");
extern struct Unk5A9C4FA94 *D_8009FA94;
extern struct Unk5A9C4Other *D_8009FA58[];
extern s32 D_8009FAA4;
extern s32 D_800A6F60[32];

extern void func_8005A740(s32 origin, s32 id, s32 colour);
extern u8 func_8005A934(struct Unk5A9C4 *object, s32 z, s32 x, s32 *angle);
extern void func_8005A828(s32 colour, s32 angle, s32 colour2);

/**
 * @brief Draws the radar markers of one object: its heading line, the
 *        other player's bearing and up to eight point markers.
 *
 * @param object Radar object; unkAC->unkC1 is the side index (0/1),
 *               unkB0 the heading, unkAC->unk300 an optional marker node.
 * @return Nothing.
 *
 * @par Confirmed
 * colour = 0x780000 when unkC1 == 1, else 0x280000. The heading line is
 * func_8005A740(colour, -unkB0 - 0x800, 0xF8F8F8).
 * When D_8009FA0C >= 2 and D_8009FA94->unk5C == 0, the other side's record
 * D_8009FA58[1 - unkC1] is tested with func_8005A934(object, unk58, unk50,
 * &angle); on success a second line func_8005A740(colour, angle, 0xF800).
 * When D_8009FAA4 & 0x180 and the marker node exists, its mask unkA4 is
 * walked in 3-bit steps (test mask 0x249249): for each active slot i,
 * func_8005A934(object, pos[i].a << 9, pos[i].b << 9, &angle) and, on
 * success, func_8005A828(colour, angle, D_800A6F60[flags[i] & 0x1F]).
 *
 * @par Likely role
 * The two colours distinguish the two sides; the 3-bit stride packs one
 * active bit plus a 5-bit color index per marker slot.
 *
 * @par Callers
 * func_8004EDC8.
 */
void func_8005A9C4(struct Unk5A9C4 *object)
{
    s32 angle;
    register s32 colour __asm__("$20");
    s32 i;
    u32 mask;
    struct Node5A9C4 *node;
    struct Unk5A9C4Other *other;
    register struct Unk5A9C4Other **base __asm__("$3");
    s32 idx;
    register s32 *colours __asm__("$22");
    register s32 high __asm__("$2");
    register struct Unk5A9C4 *arg __asm__("$4");

    colour = (object->unkAC->unkC1 == 1) ? 0x780000 : 0x280000;

    angle = -(s32)object->unkB0 - 0x800;
    func_8005A740(colour, angle, 0xF8F8F8);

    if (gPlayerCount >= 2 && D_8009FA94->unk5C == 0) {
        arg = object;
        __asm__("" : : "r" (arg));
        idx = object->unkAC->unkC1;
        __asm__("lui %0,%%hi(D_8009FA58)\n\taddiu %0,%0,%%lo(D_8009FA58)" : "=r" (base));
        other = base[1 - idx];
        if (func_8005A934(arg, other->unk58, other->unk50, &angle)) {
            func_8005A740(colour, angle, 0xF800);
        }
    }

    if (D_8009FAA4 & 0x180) {
        node = object->unkAC->unk300;
        if (node != 0) {
            mask = node->unkA4;
            i = 0;
            if (mask & 0x249249) {
                __asm__("lui %1,%%hi(D_800A6F60)\n\taddiu %0,%1,%%lo(D_800A6F60)" : "=r" (colours), "=r" (high));
                for (; i < 8; i++) {
                    if (mask & 1) {
                        if (func_8005A934(object, node->pos[i].a << 9,
                                          node->pos[i].b << 9, &angle)) {
                            func_8005A828(colour, angle,
                                          colours[node->flags[i] & 0x1F]);
                        }
                    }
                    mask >>= 3;
                }
            }
        }
    }
}
