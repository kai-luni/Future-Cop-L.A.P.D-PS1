// Categories: Graphics > Image Decode & Upload | Platform > MDEC | Resources
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
extern s32 gStripStepWidth __asm__("D_8009F4C8");
extern s32 gTransferPending __asm__("D_8009F250");
extern s32 gUploadInProgress __asm__("D_8009F9C4");
extern s32 gStripScaleEnable __asm__("D_8009F4C0");
extern State861E4 *gMovieSliceContext __asm__("D_8009F4BC");
extern void *D_8009F948, *D_8009F94C, *D_8009F954;
extern void *gWorkAreaBlock __asm__("D_8009F950");
extern void *gCharTable __asm__("D_8009FABC");
extern void *gByteTable __asm__("D_8009F9D8");
extern Block100 gScratchTable __asm__("D_800A3958");
extern Block108 gSecondScratchTable __asm__("D_800A3A58");
extern void D_80085074(void);
extern void *gAllocMemory(s32, s32) __asm__("func_80017EBC");
extern void gFreeBlock(void *) __asm__("func_800181D4");
extern void *func_8001E4C0(void *, s32, u32);
extern void gMovieSetScaleMode(s32) __asm__("func_800859D8");
extern void gMovieBuildDitherTables(void) __asm__("func_80085F1C");
extern void func_80089A88(void *, void *, s32);
extern void gMovieDecodeVlcStream(void *, void *) __asm__("func_80089BBC");
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern void *func_80092B94(void *);
extern void func_80095E90(s32);
extern void gMdecStartDecode(void *, s32) __asm__("func_80095FF4");
extern void gStartMdecOutDma(void *, s32) __asm__("func_80096070");
extern void *gMdecInSync(void *) __asm__("func_80096090");
extern void *gMdecOutSync(void *) __asm__("func_800960CC");
extern void *gMdecSetInCallback(void *) __asm__("func_80096108");
extern void *func_8009612C(void *);
s32 gDecodeMdecFrameToVram(Input861E4 *arg0, s32 arg1, u32 arg2, s32 arg3) __asm__("func_800861E4");


/**
 * @brief Decodes one image/frame with the hardware decoder and transfers it to
 *        VRAM, using a transient 0x130-byte job block and three callbacks.
 *
 * @param arg0 Pointer to a small header: u16 width, u16 height, then the
 *             compressed pixel data.
 * @param arg1 Mode byte (only the low 8 bits matter). 0 selects the plain path
 *             (pre-allocated static buffers); non-zero selects the allocating
 *             path with scratchpad copies.
 * @param arg2 x position of the destination on screen/VRAM.
 * @param arg3 y position of the destination.
 *
 * @return 1 when the transfer signalled completion, 0 otherwise (allocation
 *         failure, or the bounded wait timed out after 200000 polls).
 *
 * @par Confirmed flow
 * 1. func_80096090/800960CC/80095E90 reset three decoder/DMA channels;
 *    gMovieSetScaleMode(mode != 0) tells the uploader whether a scale pass is used.
 * 2. Buffers: in allocating mode (D_8009F9C4 != 0) func_80017EBC(0x6000) plus
 *    func_80017EBC(0x600) are taken and func_80089A88(buf1, buf2, 0) unpacks
 *    the compressed data; otherwise the static D_8009F948/D_8009F94C are used.
 * 3. The job block gMovieSliceContext = 0x130 bytes (pool mode 3 when not allocating)
 *    is zeroed and the three callback slots are filled through func_8009612C
 *    (the frame-complete hook func_80085074), gMdecSetInCallback(0) and
 *    func_80092B94(0). func_8008C9C0/8008C9E0 bracket the critical section.
 * 4. In allocating mode the two work buffers 0x10000 and 0x3000 bytes are
 *    taken; with a non-zero mode the two 0x100/0x108-byte tables D_800A3958 and
 *    D_800A3A58 are copied to the scratchpad at 0x1F800000 and 0x1F800100 and
 *    their addresses stored in D_8009FABC/D_8009F9D8 - the classic "keep the
 *    decode tables in the 1 KB scratchpad" pattern.
 * 5. Geometry: w = width * D_8009F4C8 >> 1, x = (x * D_8009F4C8) >> 1,
 *    w2 = D_8009F4C8 << 3, h/h2 = height; the destination buffer is
 *    buffer28[index].
 * 6. gMovieDecodeVlcStream(input->data, buffer) unpacks the source block,
 *    gMdecStartDecode(buffer, D_8009F4C0) starts the transfer and
 *    func_80096070(buffer24, w2 * h2 / 2) clears the work area (the /2 is the
 *    16-bit pixel size).
 * 7. The result is fetched by polling job->done up to 200000 times - the
 *    completion callback sets it.
 * 8. The cleanup path restores the callbacks and frees everything it allocated
 *    (job block, 0x6000/0x600/0x10000/0x3000 buffers) and restores
 *    D_8009F250.
 *
 * @par Globals
 * gMovieSliceContext (job block), D_8009F9C4 (allocating mode flag), D_8009F4C0
 * (scale/exit-channel mode), D_8009F4C8 (horizontal scale factor),
 * D_8009F250, D_8009F948/D_8009F94C/D_8009F950/D_8009F954 (static buffers),
 * D_8009FABC/D_8009F9D8 (scratchpad tables), D_800A3958/D_800A3A58 (the two
 * tables copied to the scratchpad).
 *
 * @par Likely role
 * The image uploader used by the front-end: it takes an .fsv/.wve frame,
 * decodes it (MDEC-style: DCT tables in the scratchpad, 16-bit output, a
 * bounded completion poll) and leaves the picture in VRAM. func_8001B4A8()
 * drains a 4-slot ring of these jobs to defer uploads to the frame end.
 *
 * @par Matching note
 * Nearly every argument is pinned to a retail register ($16-$23, $2-$8). The
 * pins are part of the byte match - "simplifying" them changes the code.
 *
 * @par Environment path
 * Decodes a compressed frame with the hardware MDEC pipeline and transfers it
 * to VRAM using a 0x130-byte gMovieSliceContext job block and completion
 * callback D_80085074. Copies decode tables gScratchTable and
 * gSecondScratchTable into scratchpad (0x1F800000/0x1F800100), invokes
 * gMovieDecodeVlcStream and gMdecStartDecode, and polls state->done.
 */
s32 gDecodeMdecFrameToVram(Input861E4 *arg0, s32 arg1, u32 arg2, s32 arg3)
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
    gMdecInSync(zero);
    gMdecOutSync(0);
    func_80095E90(0);
    __asm__("" : "+r"(buf2), "+r"(result));
    {
        register s32 flag __asm__("$4") = 0;
        if (mode & 0xFF) {
            flag = 1;
            __asm__ volatile("" : "+r"(flag));
        }
        gMovieSetScaleMode(flag);
    }
    if (gUploadInProgress) {
        buf1 = gAllocMemory(0x6000, 2);
        if (!buf1) goto cleanup;
        buf2 = gAllocMemory(0x600, 2);
        if (!buf2) goto cleanup;
        func_80089A88(buf1, buf2, 0);
    } else {
        buf1 = D_8009F948;
        buf2 = D_8009F94C;
    }
    gMovieSliceContext = gAllocMemory(0x130, gUploadInProgress ? 2 : 3);
    if (!gMovieSliceContext) goto cleanup;
    func_8001E4C0(gMovieSliceContext, 0, 0x130);
    func_8008C9C0();
    gMovieSliceContext->field12C = 1;
    gMovieSliceContext->callback120 = func_8009612C(D_80085074);
    gMovieSliceContext->callback124 = gMdecSetInCallback(0);
    gMovieSliceContext->callback128 = func_80092B94(0);
    if (gStripScaleEnable == 1) gTransferPending = gStripScaleEnable;
    func_8008C9E0();
    if (gUploadInProgress) {
        gMovieSliceContext->buffer28[0] = gAllocMemory(0x10000, 2);
        if (!gMovieSliceContext->buffer28[0]) goto cleanup;
        gMovieSliceContext->buffer24 = gAllocMemory(0x3000, 2);
        if (!gMovieSliceContext->buffer24) goto cleanup;
        {
        register u32 masked __asm__("$2") = mode & 0xFF;
        __asm__ volatile("" : "+r"(masked));
        if (masked) {
            gMovieBuildDitherTables();
            *(Block100 *)0x1F800000 = gScratchTable;
            gCharTable = (void *)0x1F800000;
            *(Block108 *)0x1F800100 = gSecondScratchTable;
            gByteTable = (void *)0x1F800100;
        }
        }
    } else {
        gMovieSliceContext->buffer28[0] = D_8009F954;
        gMovieSliceContext->buffer24 = gWorkAreaBlock;
    }
    {
        register u32 scale __asm__("$5") = gStripStepWidth;
        register u32 product __asm__("$8") = x * scale;
        register u32 width __asm__("$2") = input->w * scale;
        register State861E4 *state __asm__("$3") = gMovieSliceContext;
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
        gMovieDecodeVlcStream(input->data, state->buffer28[state->index]);
    }
    gMdecStartDecode(gMovieSliceContext->buffer28[gMovieSliceContext->index], gStripScaleEnable);
    {
        u32 width = gMovieSliceContext->w2;
        u32 height = gMovieSliceContext->h2;
        __asm__("" : "+r"(width), "+r"(height));
        gStartMdecOutDma(gMovieSliceContext->buffer24, ((s32)(width << 16) >> 16) * ((s32)(height << 16) >> 16) / 2);
    }
    gMdecOutSync(0);
    {
        register State861E4 *state __asm__("$5") = gMovieSliceContext;
        __asm__ volatile("" : "+r"(state));
        count = 0;
        while (!state->done) {
            if (count > 200000) break;
            count++;
        }
        if (count != 200000) result = 1;
    }
cleanup:
    gMdecInSync(0);
    gMdecOutSync(0);
    func_8008C9C0();
    if (gStripScaleEnable == 1) gTransferPending = 0;
    if (gMovieSliceContext) {
        func_8009612C(gMovieSliceContext->callback120);
        gMdecSetInCallback(gMovieSliceContext->callback124);
        func_80092B94(gMovieSliceContext->callback128);
    }
    func_8008C9E0();
    func_80095E90(1);
    if (gUploadInProgress) {
        if (buf1) gFreeBlock(buf1);
        if (buf2) gFreeBlock(buf2);
        if (gMovieSliceContext) {
            if (gMovieSliceContext->buffer28[0]) gFreeBlock(gMovieSliceContext->buffer28[0]);
            if (gMovieSliceContext->buffer24) gFreeBlock(gMovieSliceContext->buffer24);
            gFreeBlock(gMovieSliceContext);
            gMovieSliceContext = 0;
        }
    } else if (gMovieSliceContext) {
        gFreeBlock(gMovieSliceContext);
        gMovieSliceContext = 0;
    }
    return result;
}
