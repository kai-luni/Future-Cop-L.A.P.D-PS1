#include "game/types.h"

typedef struct { u8 data[0x100]; } Block100;
typedef struct { u8 data[0x108]; } Block108;
typedef struct {
    u16 x, y, w, h;
    u8 pad08[8];
    s32 field10;
    u8 pad14[8];
    u16 x2, y2, w2, h2;
    void *buffer24;
    void *buffer28[2];
    s32 index;
    u8 pad34[0xE8];
    volatile s32 done;
    void *callback120;
    void *callback124;
    void *callback128;
    s32 field12C;
} State861E4;

typedef struct { u16 w, h; u8 data[1]; } Input861E4;
extern s32 D_8009F9C4, D_8009F250, D_8009F4C0, D_8009F4C8;
extern State861E4 *D_8009F4BC;
extern void *D_8009F948, *D_8009F94C, *D_8009F950, *D_8009F954;
extern void *D_8009FABC, *D_8009F9D8;
extern Block100 D_800A3958;
extern Block108 D_800A3A58;
extern void D_80085074(void);
extern void *func_80017EBC(s32, s32);
extern void func_800181D4(void *);
extern void *func_8001E4C0(void *, s32, u32);
extern void func_800859D8(s32);
extern void func_80085F1C(void);
extern void func_80089A88(void *, void *, s32);
extern void func_80089BBC(void *, void *);
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern void *func_80092B94(void *);
extern void func_80095E90(s32);
extern void func_80095FF4(void *, s32);
extern void func_80096070(void *, s32);
extern void *func_80096090(void *);
extern void *func_800960CC(void *);
extern void *func_80096108(void *);
extern void *func_8009612C(void *);

s32 func_800861E4(Input861E4 *arg0, s32 arg1, u32 arg2, s32 arg3)
{
    register Input861E4 *input __asm__("$20") = arg0;
    register u32 x __asm__("$23") = arg2;
    register s32 y __asm__("$21") = arg3;
    register s32 mode __asm__("$19") = arg1;
    register void *buf1 __asm__("$17");
    register void *buf2 __asm__("$18") = 0;
    register s32 result __asm__("$22") = 0;
    u32 count;
    register void *zero __asm__("$4") = buf2;
    __asm__("" : "+r"(input), "+r"(zero));
    func_80096090(zero);
    func_800960CC(0);
    func_80095E90(0);
    __asm__("" : "+r"(buf2), "+r"(result));
    {
        register s32 flag __asm__("$4") = 0;
        if (mode & 0xFF) {
            flag = 1;
            __asm__ volatile("" : "+r"(flag));
        }
        func_800859D8(flag);
    }
    if (D_8009F9C4) {
        buf1 = func_80017EBC(0x6000, 2);
        if (!buf1) goto cleanup;
        buf2 = func_80017EBC(0x600, 2);
        if (!buf2) goto cleanup;
        func_80089A88(buf1, buf2, 0);
    } else {
        buf1 = D_8009F948;
        buf2 = D_8009F94C;
    }
    D_8009F4BC = func_80017EBC(0x130, D_8009F9C4 ? 2 : 3);
    if (!D_8009F4BC) goto cleanup;
    func_8001E4C0(D_8009F4BC, 0, 0x130);
    func_8008C9C0();
    D_8009F4BC->field12C = 1;
    D_8009F4BC->callback120 = func_8009612C(D_80085074);
    D_8009F4BC->callback124 = func_80096108(0);
    D_8009F4BC->callback128 = func_80092B94(0);
    if (D_8009F4C0 == 1) D_8009F250 = D_8009F4C0;
    func_8008C9E0();
    if (D_8009F9C4) {
        D_8009F4BC->buffer28[0] = func_80017EBC(0x10000, 2);
        if (!D_8009F4BC->buffer28[0]) goto cleanup;
        D_8009F4BC->buffer24 = func_80017EBC(0x3000, 2);
        if (!D_8009F4BC->buffer24) goto cleanup;
        {
        register u32 masked __asm__("$2") = mode & 0xFF;
        __asm__ volatile("" : "+r"(masked));
        if (masked) {
            func_80085F1C();
            *(Block100 *)0x1F800000 = D_800A3958;
            D_8009FABC = (void *)0x1F800000;
            *(Block108 *)0x1F800100 = D_800A3A58;
            D_8009F9D8 = (void *)0x1F800100;
        }
        }
    } else {
        D_8009F4BC->buffer28[0] = D_8009F954;
        D_8009F4BC->buffer24 = D_8009F950;
    }
    {
        register u32 scale __asm__("$5") = D_8009F4C8;
        register u32 product __asm__("$8") = x * scale;
        register u32 width __asm__("$2") = input->w * scale;
        register State861E4 *state __asm__("$3") = D_8009F4BC;
        register u32 height __asm__("$6") = input->h;
        register u32 xpos __asm__("$4");
        state->y = y;
        state->h = height;
        xpos = product >> 1;
        __asm__("" : "+r"(scale), "+r"(xpos), "+r"(width), "+r"(state), "+r"(height));
        state->w = width >> 1;
        state->x = xpos;
        state->x2 = xpos;
        state->y2 = y;
        state->w2 = scale << 3;
        state->h2 = height;
        state->field10 = 0;
        state->done = 0;
        func_80089BBC(input->data, state->buffer28[state->index]);
    }
    func_80095FF4(D_8009F4BC->buffer28[D_8009F4BC->index], D_8009F4C0);
    {
        u32 width = D_8009F4BC->w2;
        u32 height = D_8009F4BC->h2;
        __asm__("" : "+r"(width), "+r"(height));
        func_80096070(D_8009F4BC->buffer24, ((s32)(width << 16) >> 16) * ((s32)(height << 16) >> 16) / 2);
    }
    func_800960CC(0);
    {
        register State861E4 *state __asm__("$5") = D_8009F4BC;
        __asm__ volatile("" : "+r"(state));
        count = 0;
        while (!state->done) {
            if (count > 200000) break;
            count++;
        }
        if (count != 200000) result = 1;
    }
cleanup:
    func_80096090(0);
    func_800960CC(0);
    func_8008C9C0();
    if (D_8009F4C0 == 1) D_8009F250 = 0;
    if (D_8009F4BC) {
        func_8009612C(D_8009F4BC->callback120);
        func_80096108(D_8009F4BC->callback124);
        func_80092B94(D_8009F4BC->callback128);
    }
    func_8008C9E0();
    func_80095E90(1);
    if (D_8009F9C4) {
        if (buf1) func_800181D4(buf1);
        if (buf2) func_800181D4(buf2);
        if (D_8009F4BC) {
            if (D_8009F4BC->buffer28[0]) func_800181D4(D_8009F4BC->buffer28[0]);
            if (D_8009F4BC->buffer24) func_800181D4(D_8009F4BC->buffer24);
            func_800181D4(D_8009F4BC);
            D_8009F4BC = 0;
        }
    } else if (D_8009F4BC) {
        func_800181D4(D_8009F4BC);
        D_8009F4BC = 0;
    }
    return result;
}
