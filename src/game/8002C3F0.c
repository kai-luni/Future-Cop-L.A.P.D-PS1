// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

typedef struct {
    u8 pad00[0x11E];
    s16 unk11E;
    u8 pad120[0x15F - 0x120];
    u8 unk15F;
    u8 pad160[0x163 - 0x160];
    u8 unk163;
} Object8002C3F0;

typedef struct {
    u8 pad00[8];
    void *unk08;
} Data8002C3F0;

extern void *jtbl_8001104C[];

extern void func_800379D4(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern void func_8002BFB4(Object8002C3F0 *object, void *data);
extern void func_8002AF94(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern void func_80037B28(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern s32 gEntityBaseHandler(s32 message, Object8002C3F0 *object, Data8002C3F0 *data) __asm__("func_8002AD58");
extern void func_8002C174(Object8002C3F0 *object);
extern void func_8002B784(Object8002C3F0 *object, Data8002C3F0 *data, s32 arg3);
extern s32 func_8002ACB4(Object8002C3F0 *object, s32 value);
s32 gHandleType26Entity(s32 message, Object8002C3F0 *object, Data8002C3F0 *data, s32 arg3) __asm__("func_8002C3F0");


/**
 * @brief Message handler of object type 26: jump-table dispatch with a
 *        state-7 gate on several cases and two value queries.
 *
 * @param message Handler case 1..16 (jump table jtbl_8001104C).
 * @param object Entity block; +0x11E an s16 value, +0x15F a state byte,
 *        +0x163 a frame value.
 * @param data Case payload; case 10 compares it against 6.
 * @param arg3 Forwarded to the attack case.
 * @return Case 1: 0x168. Case 10 (data == 6): unk11E. Case 11: unk163.
 *         Case 9: the draw result. Default: the base handler result.
 *
 * @par Confirmed
 * Case 2 -> func_8002AF94; case 3 -> func_80037B28 (draw); cases 4/5/7
 * return 0 when state == 7, else func_8002C3B0 (4), base + func_8002BFB4
 * (5), or base-then-func_8002BFB4 when the base result is positive (6);
 * case 7 -> func_8002C174; case 8 -> func_8002B784 (the attack dispatcher);
 * case 9 draws via func_8002ACB4(object, unk163). Unhandled cases fall
 * through to func_8002AD58.
 *
 * @par Likely role
 * One of the numbered entity handlers registered by func_8002AE54 (case 26
 * returns this address): state 7 is a disabled/dead mode that suppresses
 * draw, tick and reaction cases.
 *
 * @par Unknown
 * What the type id 0x168 selects and the state-7 semantics.
 *
 * @par Callers
 * No direct callers in the call graph. func_8002AE54 returns the address
 * for object type 26, so the dispatcher reaches it indirectly.
 */
s32 gHandleType26Entity(s32 message, Object8002C3F0 *object, Data8002C3F0 *data, s32 arg3)
{
    Object8002C3F0 *s0;
    Data8002C3F0 *s1;
    s32 s2;
    u32 index;
    static void *const label_table[] __attribute__((used)) = {
        &&L_C438, &&L_C440, &&L_C454, &&L_C468, &&L_C488,
        &&L_C4B4, &&L_C4EC, &&L_C50C, &&L_C524, &&L_C538, &&L_C550
    };

    s0 = object;
    s1 = data;
    index = message - 1;
    if (index < 16) {
        goto *jtbl_8001104C[index];
    }
    goto default_case;

L_C438:
    return 0x168;

L_C440:
    func_8002AF94(message, s0, s1);
L_C44C:
    return 0;

L_C454:
    func_80037B28(message, s0, s1);
    return 0;

L_C468:
    if (s0->unk15F == 7) {
        return 0;
    }
    func_8002C3B0(message, s0, s1);
    return 0;

L_C488:
    if (s0->unk15F == 7) {
        return 0;
    }
    gEntityBaseHandler(message, s0, s1);
    func_8002BFB4(s0, s1->unk08);
    return 0;

L_C4B4:
    if (s0->unk15F != 7) {
        s2 = gEntityBaseHandler(message, s0, s1);
        if (s2 > 0) {
            func_8002BFB4(s0, s1->unk08);
        }
    }
    return s2;

L_C4EC:
    if (s0->unk15F == 7) {
        return 0;
    }
    func_8002C174(s0);
    return 0;

L_C50C:
    func_8002B784(s0, s1, arg3);
    return 0;

L_C524:
    return func_8002ACB4(s0, s0->unk163);

L_C538:
    if ((s32)s1 == 6) {
        return s0->unk11E;
    }
    goto default_case;

L_C550:
    return s0->unk163;

default_case:
    return gEntityBaseHandler(message, s0, s1);
}
