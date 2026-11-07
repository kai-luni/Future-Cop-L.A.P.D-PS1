#include "game/types.h"

struct Frame { u8 pad[0x16]; u8 length; u8 pad17[3]; u16 stride; };
struct State { u8 pad[8]; struct Frame *frames[2]; u8 pad10[2]; u8 count; u8 current; };
extern struct State D_800A0C30;
extern s32 D_800A0C44[];
extern s32 D_8009FAA4;
extern u8 D_8009F6B8;
extern s32 D_8009F6BC;
extern s32 D_8009F6C8;
extern void func_80024064(s32, s32, s32);
extern void func_80023F08(void);
extern void func_8001BBEC(s32, void *, s32, s32, s32);

s32 func_80029748(s32 *out)
{
    register struct State *state __asm__("$6");
    register struct Frame **frame __asm__("$3");
    register s32 index __asm__("$5");
    s32 length;
    register s32 quotient __asm__("$3");
    register u32 base __asm__("$3");
    *out = D_8009F6BC;
    if (!(D_8009F6B8 & 4)) goto return0;
    if (D_8009F6B8 & 8) return 0;
    if (!(D_8009F6B8 & 0x30)) {
        D_8009F6B8 |= 0x30;
        func_80024064(D_8009F6BC, D_800A0C44[0], 0);
        func_80023F08();
        if (D_8009F6B8 & 2) D_8009F6C8 = D_8009FAA4 - 30;
        else D_8009F6B8 &= 0xDF;
    }
    if (!(D_8009F6B8 & 2)) goto return0;
    base = 0x800A0000;
    __asm__("" : "=r"(base) : "0"(base));
    state = (struct State *)(base + 0xC30);
    quotient = (D_8009FAA4 - D_8009F6C8) / 30;
    index = quotient - 1;
    if (index >= state->count) {
        D_8009F6B8 &= 0xDF;
        if (!(D_8009F6B8 & 0x10)) D_8009F6B8 |= 8;
        return 0;
    }
    if (index == state->current) return 1;
    state->current = index;
    frame = state->frames;
    length = (*frame)->length;
    if (index >= length) {
        do {
            index -= (*frame)->length;
            frame++;
        } while (index >= length);
    }
    index = (*frame)->stride * index + 0x28;
    func_8001BBEC(0, (u8 *)*frame + index, 0x3C0, 0x1CC, 0);
    return 1;
return0:
    return 0;
}
