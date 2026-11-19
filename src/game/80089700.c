// Categories: Resources > Movie Playback > Stream Demux | Platform > CD-ROM > Streaming
#include "game/types.h"

typedef struct {
    s32 w[6];
} Src89700;

typedef struct {
    u8 pad0[0xC];
    s32 w[6];
    u8 pad1c[0x38 - 0x24];
} Ent89700;

typedef struct {
    u8 pad0[0x414];
    s32 unk414;
    s32 unk418;
    s32 unk41C;
    s32 unk420;
    volatile s32 unk424;
    volatile s32 unk428;
    s32 unk42C;
    s32 unk430;
    s32 unk434;
    s32 unk438;
    s32 unk43C;
    s32 unk440;
    u8 pad444[4];
    s32 unk448;
    s32 unk44C;
    u8 pad450[0x570 - 0x450];
    s32 unk570;
    s32 unk574;
    s32 unk578;
} Unk89700;

extern Unk89700 *gMovieStreamContext __asm__("D_8009F970");
extern u8 *gMovieStreamDescriptors __asm__("D_8009F974");
extern u8 *gMovieRingStart __asm__("D_8009F978");
extern u8 *gMovieRingEnd __asm__("D_8009F97C");
extern s32 D_8009F980;
extern s32 gMovieRingBytes __asm__("D_8009F984");
extern s32 gMovieDescriptorCount __asm__("D_8009F988");

extern void *gFillMemory(void *dst, s32 val, u32 count) __asm__("func_8001E4C0");
extern s32 func_8008E5D8(s32 cb);
extern s32 func_80091240(s32 v);
extern void gMovieStreamPrimeEntry(void) __asm__("func_80088F00");
extern void gCdStreamService(s32 arg) __asm__("func_800895E4");

/**
 * @brief Initializes the movie-stream demux context and starts CD streaming playback.
 *
 * Zeroes the Unk89700 context arg0, binds gMovieStreamContext/74/78/7C pointers, copies each
 * arg3 24-byte descriptor into the +12 field of a 56-byte entry, saves existing DMA3 and CD
 * callbacks into unk574/unk578, primes the first entry, and enables CD streaming.
 *
 * @param arg0 Caller-supplied buffer holding the Unk89700 header and ring/entry storage.
 * @param arg1 Callback pointer stored in arg0->unk570.
 * @param arg2 Array of arg3 24-byte Src89700 stream descriptors.
 * @param arg3 Number of stream descriptors to copy into gMovieStreamDescriptors.
 * @return Always 1.
 *
 * @par Globals
 * gMovieStreamContext..gMovieRingEnd - active context, entries, and ring bounds; D_8009F980/84/88 - ring/entry sizing.
 *
 * @par Control flow
 * Clears arg0, copies arg3 descriptors in a for-loop, swaps out callbacks via func_8008E5D8(0) and func_80091240(0), then calls gMovieStreamPrimeEntry and gCdStreamService(1).
 *
 * @par Calls
 * func_8001E4C0 - zeroes context; func_8008E5D8, func_80091240 - save/clear DMA3/CD hooks; gMovieStreamPrimeEntry - primes first stream entry; gCdStreamService - starts CD stream service.
 *
 * @par Side effects
 * Initializes gMovieStreamContext..gMovieRingEnd, clears/saves CD/DMA3 callbacks, and starts CD stream mode 0xA0.
 *
 * @par Callers
 * Confirmed: gPlayVideoFile (0x800883FC in asm/us/main.s; edges.json lists start +4 at 0x80088124).
 */
s32 gMovieStreamReaderInit(Unk89700 *arg0, s32 arg1, Src89700 *arg2, u32 arg3) __asm__("func_80089700");

s32 gMovieStreamReaderInit(Unk89700 *arg0, s32 arg1, Src89700 *arg2, u32 arg3)
{
    u32 i;
    s32 t;
    u8 *p;

    t = (D_8009F980 << 11) + 0x57C;
    gFillMemory(arg0, 0, gMovieRingBytes + t + gMovieDescriptorCount * 56);
    gMovieStreamContext = arg0;
    p = (u8 *)arg0 + 0x57C;
    gMovieStreamDescriptors = p;
    arg0->unk570 = arg1;
    arg0->unk414 = 0;
    arg0->unk418 = 0;
    arg0->unk41C = 0;
    arg0->unk43C = 0;
    arg0->unk434 = 0;
    arg0->unk430 = 0;
    arg0->unk42C = 0;
    arg0->unk440 = 0;
    arg0->unk448 = 0;
    arg0->unk44C = 0;
    gMovieRingStart = p + gMovieDescriptorCount * 56;
    gMovieRingEnd = gMovieRingStart + gMovieRingBytes;
    for (i = 0; i < arg3; i++) {
        *(Src89700 *)(gMovieStreamDescriptors + i * 56 + 12) = arg2[i];
    }
    gMovieStreamContext->unk438 = arg3;
    gMovieStreamContext->unk424 = 0;
    gMovieStreamContext->unk428 = 0;
    gMovieStreamContext->unk574 = func_8008E5D8(0);
    gMovieStreamContext->unk578 = func_80091240(0);
    gMovieStreamPrimeEntry();
    gCdStreamService(1);
    return 1;
}
