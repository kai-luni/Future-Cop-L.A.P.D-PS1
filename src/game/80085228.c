// Categories: Resources > Movie Playback | Graphics > Video Decode > Frame Slice Upload | Platform > MDEC | Platform > DMA
#include "game/types.h"

struct State {
    volatile u16 field00;
    volatile u16 field02;
    volatile u16 field04;
    volatile u16 field06;
    volatile u16 field08;
    volatile u16 field0A;
    u16 field0C;
    volatile u16 field0E;
    volatile s32 field10;
    u8 pad14[0x08];
    volatile u16 field1C;
    volatile u16 field1E;
    volatile u16 field20;
    volatile u16 field22;
    s32 field24;
    s32 field28[2];
    s32 field30;
    u8 pad34[0xE4];
    s32 field118;
    volatile s32 field11C;
};

extern void *memcpy(void *, const void *, u32);

extern struct State *gDecodeJob __asm__("D_8009F4BC");
extern s32 gStripScaleEnable __asm__("D_8009F4C0");
extern s32 D_8009F4C4;
extern s32 gStripStepWidth __asm__("D_8009F4C8");
extern u32 gFrameTimeMax __asm__("D_8009F4D0");
extern u8 *gCharTable __asm__("D_8009FABC");
extern u8 *gByteTable __asm__("D_8009F9D8");
extern u8 gScratchTable[0x100] __asm__("D_800A3958");
extern u8 gSecondScratchTable[0x108] __asm__("D_800A3A58");

extern void func_80089BBC(u8 *arg0, s32 arg1);
extern void func_80095E90(s32 option);
extern u32 func_80095FE8(u16 *value);
extern void func_80095FF4(s32 arg0, s32 arg1);
extern void func_80096070(s32 destination, u32 size);
extern s32 func_800960CC(s32 mode);

/**
 * @brief Decodes and uploads one video/frame slice on the interrupt-driven path,
 *        where the registered decode callbacks perform the transfer.
 *
 * @param data Slice header in big-endian order: a four-byte tag, a 16-bit size at
 *             +0x08 and a second 16-bit size at +0x0A; the payload starts at +0x10.
 * @param input_arg1 Slice size; multiplied by the rate at D_8009F4C8 and halved
 *             before it becomes the width of the descriptor quads.
 * @param input_arg2 Vertical position/volume; stored in the descriptor and offset
 *             by 0x100 when the context flag at +0x114 is set.
 *
 * @return Nothing.
 *
 * @par Confirmed control flow
 * 1. The tag and both 16-bit fields are byte-swapped into one 32-bit magic value
 *    that is compared with 0x4D444543; payload = data + 0x10.
 * 2. When mode D_8009F4C0 equals 1 and D_8009F4C4 is non-zero, the two decoder
 *    tables are copied into the scratchpad - 0x100 bytes from D_800A3958 to
 *    0x1F800000 and 0x108 bytes from D_800A3A58 to 0x1F800100 - and D_8009FABC and
 *    D_8009F9D8 are repointed at those copies.
 * 3. func_80095FE8(payload) reads the first half-word of the payload and raises the
 *    running maximum in D_8009F4D0.
 * 4. The context descriptor is written as three quads of four half-words:
 *    +0x00 = (arg1 * rate) >> 1, +0x02 = arg2, +0x04 = (size8 * rate) >> 1,
 *    +0x06 = sizeA; +0x08 = (arg1 * rate) >> 1, +0x0A = arg2 + 0x100,
 *    +0x0C = (size8 * rate) >> 1, +0x0E = sizeA; +0x10 = 0; and +0x1C =
 *    (arg1 * rate) >> 1, +0x1E = arg2, +0x20 = rate << 3, +0x22 = sizeA.
 * 5. First call (context +0x118 == 0): no transfer; when the tag matched,
 *    func_80089BBC(payload, ctx+0x28[ctx+0x30]) decodes into the current buffer, and
 *    +0x11C is cleared.
 * 6. Later calls: +0x11C is cleared, func_80095FF4(ctx+0x28[1 - ctx+0x30], mode)
 *    prepares the other buffer, func_80096070(ctx+0x24, width * height / 2) starts
 *    the transfer, the optional decode follows, func_800960CC(0) runs, and a bounded
 *    loop of 0x30D40 (200000) iterations waits for the callback to raise +0x11C.
 * 7. Both paths end with func_80095E90(1), then +0x118 is incremented and
 *    +0x30 = 1 - +0x30, so consecutive slices alternate buffers.
 *
 * @par Likely role
 * The callback twin of func_80085630: func_8008589C routes every slice here when the
 * context's callback mode (+0x12C) is set, and to func_80085630 when it is not. The
 * slice header, the tag test, the double buffer, the scratchpad tables and the
 * counter/flip tail are identical; the difference is that this variant hands the
 * whole block to one DMA transfer and then waits for the interrupt, while
 * func_80085630 walks the rows itself and sets +0x11C directly.
 *
 * @par Unknown
 * - The exact meaning of the three descriptor quads: they repeat the same scaled
 *   values plus a stride, which fits VRAM destination rectangles, but no consumer is
 *   visible here.
 * - The unit of the 16-bit payload field read by func_80095FE8, kept only as a
 *   running maximum.
 *
 * @par Callers
 * func_8008589C (the slice dispatcher, call site 0x800858A0).
 */
void func_80085228(u8 *data, s32 input_arg1, s32 input_arg2)
{
    register s32 arg1 __asm__("$21");
    register s32 arg2 __asm__("$18");
    register u8 *p __asm__("$9");
    register u8 *payload __asm__("$16");
    register u32 header __asm__("$5");
    register u32 rawA __asm__("$3");
    register u32 raw8 __asm__("$2");
    register u32 magic __asm__("$20");
    register u32 valA __asm__("$17");
    register u32 val8 __asm__("$19");
    register u32 value __asm__("$2");
    u32 i;
    register struct State *state __asm__("$6");

    arg1 = input_arg1;
    arg2 = input_arg2;
    p = data + 8;
    header = *(u32 *)data;
    __asm__("" : "=r" (arg1), "=r" (arg2), "=r" (p) : "0" (arg1), "1" (arg2), "2" (p));
    rawA = *(u16 *)(p + 2);
    __asm__("" : "=r" (rawA), "=r" (header) : "0" (rawA), "1" (header));

    state = (struct State *)(header >> 24);
    raw8 = (header >> 8) & 0xFF00;
    __asm__("" : "=r" (raw8) : "0" (raw8));
    state = (struct State *)((u32)state | raw8);
    raw8 = (header & 0xFF00) << 8;
    __asm__("" : "=r" (raw8) : "0" (raw8));
    state = (struct State *)((u32)state | raw8);
    header = header << 24;
    __asm__("" : "=r" (header) : "0" (header));
    magic = (u32)state | header;
    header = rawA >> 8;
    __asm__("" : "=r" (header) : "0" (header));
    rawA = (rawA & 0xFF) << 8;
    __asm__("" : "=r" (rawA) : "0" (rawA));
    raw8 = *(u16 *)(data + 8);
    valA = header | rawA;
    rawA = raw8 >> 8;
    __asm__("" : "=r" (rawA) : "0" (rawA));
    raw8 = (raw8 & 0xFF) << 8;
    val8 = rawA | raw8;
    __asm__("" : "=r" (p), "=r" (val8) : "0" (p), "1" (val8) : "memory");

    {
    register s32 mode __asm__("$3");
    register s32 one __asm__("$2");
    mode = gStripScaleEnable;
    __asm__("" : "=r" (mode), "=r" (p) : "0" (mode), "1" (p));
    one = 1;
    __asm__("" : "=r" (one), "=r" (p) : "0" (one), "1" (p));
    payload = p + 8;
    if (mode == one) {
        if (D_8009F4C4 != 0) {
            memcpy((void *)0x1F800000, gScratchTable, 0x100);
            gCharTable = (u8 *)0x1F800000;
            memcpy((void *)0x1F800100, gSecondScratchTable, 0x108);
            gByteTable = (u8 *)0x1F800100;
            payload = p + 8;
        }
    }
    }


    value = func_80095FE8((u16 *)payload);
    if (gFrameTimeMax < value) {
        gFrameTimeMax = value;
    }

    state = gDecodeJob;
    if (state->field118 == 0) {
        register s32 scale __asm__("$4");
        register u32 tag __asm__("$5");
        register u32 half __asm__("$3");
        tag = 0x4D444543;

        scale = gStripStepWidth;

        state->field02 = (u16)arg2;
        state->field06 = valA;
        state->field0A = (u16)(arg2 + 0x100);
        state->field0E = valA;

        state->field00 = (u16)((u32)(arg1 * scale) >> 1);
        state->field08 = (u16)((u32)(arg1 * scale) >> 1);
        state->field1C = (u16)((u32)(arg1 * scale) >> 1);
        state->field1E = (u16)arg2;
        state->field20 = (u16)(scale << 3);
        state->field22 = valA;
        state->field10 = 0;

        half = (u32)(val8 * scale);
        __asm__("" : "=r" (half) : "0" (half));
        half >>= 1;
        state->field04 = (u16)half;
        __asm__("" : : : "memory");
        state->field0C = (u16)half;

        if (magic == tag) {
            func_80089BBC(payload,
                          state->field28[state->field30]);
        }
        gDecodeJob->field11C = 0;
    } else {
        s32 prod;

        state->field11C = 0;
        func_80095FF4(state->field28[1 - state->field30], gStripScaleEnable);

        {
            u16 v20 = *(volatile u16 *)&gDecodeJob->field20;
            u16 v22 = *(volatile u16 *)&gDecodeJob->field22;

            prod = (s32)(s16)v20 * (s32)(s16)v22;
        }
        func_80096070(gDecodeJob->field24, (u32)(prod / 2));

        if (magic == 0x4D444543) {
            func_80089BBC(payload,
                          gDecodeJob->field28[gDecodeJob->field30]);
        }

        func_800960CC(0);

        {
            register struct State *sp __asm__("$4");

            sp = gDecodeJob;
            i = 0;
            while (sp->field11C == 0) {
                if (i > 0x30D40) {
                    break;
                }
                i++;
            }
        }
    }

    func_80095E90(1);
    {
        register struct State *tp __asm__("$3");

        tp = gDecodeJob;
        tp->field118++;
        tp->field30 = 1 - tp->field30;
    }
}
