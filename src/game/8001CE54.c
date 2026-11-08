#include "game/types.h"

#define LOAD_ADDRESS(p, sym) __asm__("lui\t%0,%%hi(" #sym ")\n\taddiu\t%0,%0,%%lo(" #sym ")" : "=r"(p))

typedef struct {
    volatile s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
} GlobalState;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
} Entry;

extern GlobalState func_80010000;
extern Entry D_800A6D78[];
extern s32 D_8009F244;
extern u8 D_8009F248;
extern s32 D_8009F618;
extern u8 D_8009F610[];
extern u8 D_8009F614[];
extern u8 D_8009F61C[];
extern u8 D_800A0798[];
extern u8 D_8001C60C[];
extern u8 D_8001C614[];
extern volatile u8 D_1F801800;
extern volatile u8 D_1F801803;

extern s32 func_8001C478(s32 mode);
extern void func_8001CC6C(void);
extern void func_8001C494(u8 *dst, s32 arg1);
extern s32 func_8001C3F8(u8 *a, u8 *b);
extern void func_8001C27C(void *dst, void *src);
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern s32 func_8008E174(s32 mode, u8 *result);
extern s32 func_8008E194(s32 ptr);
extern s32 func_8008E1BC(s32 mode, void *src, void *dst);

s32 func_8001CE54(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    Entry *entry;
    GlobalState *st;
    s32 kind;
    register s32 result __asm__("$2");
    register GlobalState *liveState __asm__("$17");
    s32 previous;
    register s32 index __asm__("$18");
    register s32 base __asm__("$2");
    register s32 offset __asm__("$3");
    register s32 callback __asm__("$19");
    register u32 stateHigh __asm__("$16");
    register GlobalState *oldState __asm__("$2");
    register s32 second __asm__("$21");
    register s32 third __asm__("$20");
    register s32 fifth __asm__("$5");
    register u8 *a __asm__("$4");
    register u8 *b __asm__("$5");

    index = arg0;
    second = arg1;
    third = arg2;
    __asm__("" : : "r"(second), "r"(third));
    __asm__("" : : : "$19");
    callback = arg3;
    base = (s32)D_800A6D78;
    offset = index * 24;
    entry = (Entry *)(offset + base);
    kind = entry->unk10;
    if (kind != 1 && kind != 0x11) {
        return -7;
    }
    result = func_8001C478(0x11);
    if (result < 0) {
        return result;
    }
    entry->unk00 = 2;
    st = &func_80010000;
    st->unk14 = callback;
    if (callback == 0) {
        st->unk14 = (s32)D_8001C60C;
    }
    st->unk04 = third;
    st->unk10 = index;
    st->unk20 = second;
    st->unk08 = 0;
    if ((entry->unk0C & 0xFFFFF800) == D_8009F244) {
        func_8001CC6C();
    }
    if (st->unk04 <= 0) {
        goto success;
    }
    fifth = entry->unk0C;
    LOAD_ADDRESS(a, D_8009F614);
    func_8001C494(a, fifth);
    while (func_8008E174(1, 0) != 0) {
    }
    a = (u8 *)0xE;
    b = D_8009F610;
    __asm__("lui\t$2,%%hi(D_800A0798)\n\taddiu\t%0,$2,%%lo(D_800A0798)" : "=r"(index) : "r"(a), "r"(b) : "$2");
    D_1F801800 = 0;
    D_1F801803 = 0;
    __asm__ volatile("" : "+r"(index) : : "memory");
    result = func_8008E1BC((s32)a, b, (void *)index);
    if (result == 0) {
        stateHigh = 0x80010000;
        __asm__("addiu\t%0,%1,0" : "=r"(oldState) : "r"(stateHigh));
        previous = oldState->unk1C;
        goto failure;
    }
    if (D_8009F618 == 0) {
        func_8008C9C0();
    }
    LOAD_ADDRESS(a, D_8009F61C);
    LOAD_ADDRESS(b, D_8009F614);
    if (func_8001C3F8(a, b) == 0) {
        func_80010000.unk00 = 2;
        if (D_8009F618 != 0) {
            goto success;
        }
        func_8008C9E0();
        goto success;
    }
    stateHigh = 0x80010000;
    ((GlobalState *)stateHigh)->unk00 = 1;
    __asm__ volatile(
        "lw\t$2,D_8009F618\n\t"
        "nop\n\t"
        ".set\tnoreorder\n\t"
        ".set\tnomacro\n\t"
        "bne\t$2,$0,1f\n\t"
        "addiu\t$17,$16,0\n\t"
        "jal\tfunc_8008C9E0\n\t"
        "nop\n\t"
        ".set\tmacro\n\t"
        ".set\treorder\n"
        "1:"
        : "=r"(liveState)
        : "r"(stateHigh)
        : "$2"
    );
    liveState->unk1C = func_8008E194((s32)D_8001C614);
    LOAD_ADDRESS(a, D_8009F61C);
    LOAD_ADDRESS(b, D_8009F614);
    func_8001C27C(a, b);
    LOAD_ADDRESS(b, D_8009F614);
    if (func_8008E1BC(D_8009F248, b, (void *)index) != 0) {
        goto success;
    }
    previous = liveState->unk1C;
failure:
    func_8008E194(previous);
    result = -4;
    __asm__ volatile("sw\t$0,0(%0)" : : "r"(stateHigh), "r"(result) : "memory");
    return result;
success:
    oldState = &func_80010000;
    __asm__("" : "+r"(oldState));
    return oldState->unk08;
}
