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

extern Unk89700 *gQueueBlock __asm__("D_8009F970");
extern u8 *gStreamDescriptors __asm__("D_8009F974");
extern u8 *D_8009F978;
extern u8 *D_8009F97C;
extern s32 D_8009F980;
extern s32 D_8009F984;
extern s32 D_8009F988;

extern void *func_8001E4C0(void *dst, s32 val, u32 count);
extern s32 func_8008E5D8(s32 cb);
extern s32 func_80091240(s32 v);
extern void func_80088F00(void);
extern void func_800895E4(s32 arg);

/**
 * @brief Initializes the movie-stream demux context and starts CD streaming playback.
 *
 * Zeroes the Unk89700 context arg0, binds D_8009F970/74/78/7C pointers, copies arg3
 * 24-byte stream descriptors into the 56-byte entry array, saves existing DMA3 and CD
 * callbacks into unk574/unk578, primes the first entry, and enables CD streaming.
 *
 * @param arg0 Caller-supplied buffer holding the Unk89700 header and ring/entry storage.
 * @param arg1 Callback pointer stored in arg0->unk570.
 * @param arg2 Array of arg3 24-byte Src89700 stream descriptors.
 * @param arg3 Number of stream descriptors to copy into D_8009F974.
 * @return Always 1.
 *
 * @par Globals
 * D_8009F970..D_8009F97C - active context, entries, and ring bounds; D_8009F980/84/88 - ring/entry sizing.
 *
 * @par Control flow
 * Clears arg0, copies arg3 descriptors in a for-loop, swaps out callbacks via func_8008E5D8(0) and func_80091240(0), then calls func_80088F00 and func_800895E4(1).
 *
 * @par Calls
 * func_8001E4C0 - zeroes context; func_8008E5D8, func_80091240 - save/clear DMA3/CD hooks; func_80088F00 - primes first stream entry; func_800895E4 - starts CD stream service.
 *
 * @par Side effects
 * Initializes D_8009F970..D_8009F97C, clears/saves CD/DMA3 callbacks, and starts CD stream mode 0xA0.
 *
 * @par Callers
 * Confirmed: func_80088120 (0x800883FC in asm/us/main.s; edges.json lists start +4 at 0x80088124).
 */
s32 func_80089700(Unk89700 *arg0, s32 arg1, Src89700 *arg2, u32 arg3)
{
    u32 i;
    s32 t;
    u8 *p;

    t = (D_8009F980 << 11) + 0x57C;
    func_8001E4C0(arg0, 0, D_8009F984 + t + D_8009F988 * 56);
    gQueueBlock = arg0;
    p = (u8 *)arg0 + 0x57C;
    gStreamDescriptors = p;
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
    D_8009F978 = p + D_8009F988 * 56;
    D_8009F97C = D_8009F978 + D_8009F984;
    for (i = 0; i < arg3; i++) {
        *(Src89700 *)(gStreamDescriptors + i * 56 + 12) = arg2[i];
    }
    gQueueBlock->unk438 = arg3;
    gQueueBlock->unk424 = 0;
    gQueueBlock->unk428 = 0;
    gQueueBlock->unk574 = func_8008E5D8(0);
    gQueueBlock->unk578 = func_80091240(0);
    func_80088F00();
    func_800895E4(1);
    return 1;
}
