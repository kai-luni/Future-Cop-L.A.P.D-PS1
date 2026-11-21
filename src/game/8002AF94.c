// Categories: Gameplay > Object Setup | Gameplay > Entity Script | Graphics > Animation
/* Exact .text reconstruction with GCC's labels-as-values extension.
 * jtbl_80010FAC is the pre-existing external retail dispatch table. Its eight
 * entries are deliberately not guessed or replaced here.
 * possible_targets retains the locally supplied indirect-entry blocks; it
 * creates eight additional, unreferenced bytes in .rodata, outside .text.
 * Only the supplied .text oracle and all-zero .text padding are byte-verified.
 */
#include "game/types.h"

typedef struct {
    u8 pad48[0x48];
    u8 f48;
    u8 f49;
    u8 f4a;
    u8 f4b;
    u8 f4c;
    u8 pad4d;
    u8 f4e;
    u8 f4f;
} Def8002AF94;

typedef struct {
    u8 pad08[8];
    Def8002AF94 *def;
} S08002AF94;

extern void func_800382B4(void *arg);
extern void func_800448D8(u8 *arg0, s32 arg1, s32 arg2, s32 arg3);
extern u32 func_80019668(void);
extern void *jtbl_80010FAC[8];
void gSetupType26Entity(void *a0, u8 *s1, S08002AF94 *s0) __asm__("func_8002AF94");


/**
 * @brief Handles message-2 spawn setup for entity class 26.
 *
 * Forwards base setup to func_800382B4, attaches clip 0 on animation track +0xC8, copies class-26 state and animation variants from s0->def (+0x48..+0x4F) into s1 + 0x158..+0x167, randomizes lock range +0x15A, and clamps +0x10C/+0x10E for active modes.
 *
 * @param a0 Message id forwarded in $a0 to func_800382B4 (alongside s1 in $a1 and s0 in $a2).
 * @param s1 Class-26 actor block (+0xAA range limit, +0xC8 track, +0x10C/+0x10E counters, +0x158..+0x167 behavior/clip fields).
 * @param s0 Spawn descriptor whose +0x08 pointer supplies byte fields at +0x48..+0x4F.
 *
 * @par Globals
 * None (uses jump table jtbl_80010FAC for the +0x15F mode switch).
 *
 * @par Control flow
 * Runs setup calls linearly, computes +0x15A = aa - (PRNG % ((s16)aa >> 1)), then switches on s1[0x15F] (< 8) via jtbl_80010FAC: modes 0 and 3 halve +0x10C twice (>> 2) if !(s1[0x160] & 1) and clamp +0x10E <= +0x10C; modes 1, 2, 4..7 and >= 8 skip clamping.
 *
 * @par Calls
 * func_800382B4 (base class-5 message-2 setup), func_800448D8 (selects clip 0 on track s1 + 0xC8), func_80019668 (random jitter for lock range +0x15A).
 *
 * @par Side effects
 * Initializes s1 + 0xC8 track, +0x158..+0x167 actor state/clip bytes, +0x15A range, and optionally scales +0x10C/+0x10E.
 *
 * @par Callers
 * Confirmed: func_8002C3F0 (0x8002C444, message 2 of entity class 26).
 */
void gSetupType26Entity(void *a0, u8 *s1, S08002AF94 *s0)
{
    Def8002AF94 *def;
    u32 r;
    u16 aa;
    u16 t;
    s32 d;
    u8 field4e;

    func_800382B4(a0);
    *(u8 *)(s1 + 0x15E) = 0;
    func_800448D8(s1 + 0xC8, 0, 1, 0);

    def = s0->def;
    *(u8 *)(s1 + 0x160) = def->f4f;
    *(u8 *)(s1 + 0x15F) = def->f48;
    *(u8 *)(s1 + 0x162) = 0;
    *(u8 *)(s1 + 0x163) = def->f49 | 1;
    *(u8 *)(s1 + 0x164) = def->f4a;
    *(u8 *)(s1 + 0x165) = def->f4b;
    *(u8 *)(s1 + 0x166) = def->f4c;
    t = *(u16 *)(s1 + 0x10C);
    field4e = def->f4e;
    *(u16 *)(s1 + 0x158) = t;
    *(u8 *)(s1 + 0x167) = field4e;

    r = func_80019668();
    aa = *(u16 *)(s1 + 0xAA);
    d = (s32)(s16)aa >> 1;
    *(u16 *)(s1 + 0x15A) = (u16)(aa - (r % (u32)d));

    {
        static void *const possible_targets[] = {&&active, &&done};
        u32 kind = *(u8 *)(s1 + 0x15F);
        if (kind < 8) goto *jtbl_80010FAC[kind];
        goto done;
    }
active:
        if ((*(u8 *)(s1 + 0x160) & 1) == 0) {
            *(s16 *)(s1 + 0x10C) = (s16)*(u16 *)(s1 + 0x10C) >> 2;
        }
        if (*(s16 *)(s1 + 0x10E) > *(s16 *)(s1 + 0x10C)) {
            *(u16 *)(s1 + 0x10E) = *(u16 *)(s1 + 0x10C);
        }
done:
    return;
}
