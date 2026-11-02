#include "game/types.h"

struct SoundBank {
    s32 samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    s32 head;
};

extern u8 D_800A1830[];
extern struct SoundBank D_800A7938[];

extern s32 func_80045494(s32 id, u8 *table, s32 limit);
extern s32 func_800458E8(s32 arg0, s32 arg1, s8 *mix, void *arg3);
extern s32 func_80046B04(s32 volume, s32 arg1, s32 arg2, u32 index);
extern void func_800460D4(s8 *mix);
extern u8 func_8004772C(struct SoundBank *bank, s32 handle, s32 pan, s32 volume, s32 extra);
extern void func_80046698(s32 handle);

s32 func_80046E88(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object)
{
    register s32 result __asm__("$16");
    register s32 slot __asm__("$17");
    struct SoundBank *bank;
    s8 mix[0x18];

    result = 0;
    __asm__("" : "=r"(result) : "0"(result));
    slot = func_80045494(track, D_800A1830, 0x47);
    __asm__("" : "=r"(slot) : "0"(slot));
    if (slot == 0xFF) {
        return result;
    }
    bank = &D_800A7938[slot];
    if (func_800458E8(arg1, arg2, mix, object) == 0) {
        return result;
    }
    if (handle == 0) {
        result = func_80046B04(mix[0], bank->unk13, result, slot);
        if (result == 0) {
            goto done;
        }
        handle = result;
    }
    func_800460D4(mix);
    if (func_8004772C(bank, handle, mix[1], mix[0], arg3) != 0) {
        return result;
    }
    if (result != 0) {
        func_80046698(result);
    }
    result = 0;
    __asm__("" : "=r"(result) : "0"(result));
done:
    return result;
}
