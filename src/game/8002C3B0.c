// Categories: Gameplay > Entity Update | Gameplay > Collision Response
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
extern s32 func_8002AD58(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern void func_8002C174(Object8002C3F0 *object);
extern void func_8002B784(Object8002C3F0 *object, Data8002C3F0 *data, s32 arg3);
extern s32 func_8002ACB4(Object8002C3F0 *object, s32 value);

/**
 * @brief Handles contact/target update for type-26 entities by combining family collision response and target acquisition.
 *
 * Forwards the message and partner object to func_800379D4 (which updates relative speed/timers and notifies func_8002AD58) and then evaluates aggro/target selection via func_8002BFB4. Category Gameplay > Entity Update | Gameplay > Collision Response follows object state offsets (+0x118..+0x160) and caller func_8002C3F0.
 *
 * @param message Entity message id (dispatched from case 4 in func_8002C3F0).
 * @param object Receiving `Object8002C3F0` entity block.
 * @param data Contact/target partner object.
 *
 * @par Globals
 * None directly.
 *
 * @par Control flow
 * Straight-line sequence of two calls with no local branches or loops.
 *
 * @par Calls
 * func_800379D4 - resolves same-family contact state/speed updates and conditionally delegates to func_8002AD58.
 * func_8002BFB4 - updates target pointers (+0xB8/+0x138), flags (+0x118), and state (+0x15F) from `data`.
 *
 * @par Side effects
 * Mutates contact timers, speeds, target pointers, and state fields on `object` and `data` via callees.
 *
 * @par Callers
 * Confirmed: func_8002C3F0 (0x8002C478 in asm/us/main.s, case 4 when unk15F != 7) / Likely: none / Unknown: none.
 */
void func_8002C3B0(s32 message, Object8002C3F0 *object, Data8002C3F0 *data)
{
    func_800379D4(message, object, data);
    func_8002BFB4(object, data);
}
