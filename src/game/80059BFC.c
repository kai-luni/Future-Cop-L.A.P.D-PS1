// Categories: Resources > File Access | Game Engine > Init | Input > Demo Playback
#include "game/types.h"

typedef struct {
    u8 unk00;
    s8 unk01;
    s8 unk02;
    s8 unk03;
    s8 unk04;
    s8 unk05;
    s8 unk06;
    s8 unk07;
    s16 sizes[2];
    u8 *ptrs1[2];
    u8 *ptrs2[2];
    u8 *end1;
    u8 *end2;
    u8 data[1];
} LoadedData;

typedef struct {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x1C];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[0x58];
    s32 unk98;
} SomeState;

extern s32 gDemoPlayback __asm__("D_8009FA84");
extern SomeState *gGameSession __asm__("D_8009FA94");
extern LoadedData *gStreamDescriptorPtr __asm__("D_8009F7D0");
extern u8 gDemoPlaybackFilenameFormat[] __asm__("D_80011B00");

extern void gSeedPrng(s32 seed) __asm__("func_800197A8");
extern s32 func_8008CCC0(u8 *str);
extern void func_8008CD30(u8 *dst, u8 *fmt, u8 *name, s32 arg3, s32 arg4);
extern void *gLoadFileIntoBuffer(s32 name) __asm__("func_80059BA0");
void gLoadDemoRecording(s32 countArg, s32 seed, u8 *nameArg) __asm__("func_80059BFC");


/**
 * @brief Seeds the RNG, formats a mission demo filename, loads the file, and partitions its stream buffer.
 *
 * Confirmed: calls gSeedPrng(seed) and returns early if D_8009FA84 <= 0. Uses func_8008CCC0 to strip any trailing ".mis" on nameArg, formats buffer via func_8008CD30 with D_80011B00 ("%s_%c%d.dem"), nameArg, countArg + 0x40, and D_8009FA84, restores '.', and loads via func_80059BA0 into D_8009F7D0. On success, copies data->unk01..unk07 into D_8009FA94 (+0x08/+0x98/+0x28..+0x3C), sets ptrs1/ptrs2[0..countArg-1] across data->data advancing by data->sizes[i] * 4, and sets end1/end2; on failure, clears D_8009FA84 = 0. Category follows file load func_80059BA0, ".dem" extension, and mission init caller.
 * Likely: loads A/B demo input recordings during mission boot (countArg 1 or 2 -> 'A' or 'B'). Unknown: exact meaning of header bytes unk01..unk07 in D_8009FA94.
 *
 * @param countArg Number of stream pairs to slice (1 or 2; +0x40 gives 'A' or 'B').
 * @param seed RNG seed passed to func_800197A8.
 * @param nameArg Mission name string (temporarily modified in place if ending in ".mis").
 * @par Globals
 * D_8009FA84 (demo id/mode, cleared on load failure), D_80011B00 (format "%s_%c%d.dem"), D_8009F7D0 (loaded buffer ptr), D_8009FA94 (header bytes copied).
 * @par Control flow
 * Early exit if D_8009FA84 <= 0; conditional ".mis" strip/restore; null check on loaded buffer with counted do-while slicing streams.
 * @par Calls
 * func_800197A8 (seed RNG), func_8008CCC0 (strlen), func_8008CD30 (sprintf), func_80059BA0 (file load).
 * @par Side effects
 * Temporarily modifies and restores nameArg, allocates/loads D_8009F7D0 buffer, writes D_8009FA94 header fields, or clears D_8009FA84.
 * @par Callers
 * Confirmed: func_80024A24 (0x80024CA4, during mission boot when D_8009FA84 != 0) / Likely: none / Unknown: none.
 */
void gLoadDemoRecording(s32 countArg, s32 seed, u8 *nameArg)
{
    u8 buffer[0x100];
    register s32 count __asm__("$19") = countArg;
    register u8 *name __asm__("$16") = nameArg;
    register s32 len __asm__("$17");
    register s32 wasExt __asm__("$18");
    register LoadedData *data __asm__("$6");
    register SomeState *state __asm__("$3");
    register u8 *ptr __asm__("$5");
    register s32 i __asm__("$4");

    gSeedPrng(seed);
    if (gDemoPlayback <= 0) {
        return;
    }

    len = func_8008CCC0(name);
    wasExt = 0;
    if (name[len - 4] == '.' && name[len - 3] == 'm' &&
        name[len - 2] == 'i' && name[len - 1] == 's') {
        wasExt = 1;
        name[len - 4] = 0;
    }

    func_8008CD30(buffer, gDemoPlaybackFilenameFormat, name, count + 0x40, gDemoPlayback);
    if (wasExt) {
        register u8 *restore __asm__("$3") = (u8 *)(len + (s32)name);
        restore[-4] = '.';
    }

    data = gLoadFileIntoBuffer((s32)buffer);
    gStreamDescriptorPtr = data;
    if (data != 0) {
        ptr = data->data;
        state = gGameSession;
        state->unk08 = data->unk01;
        state->unk98 = data->unk01;
        state->unk28 = data->unk02;
        state->unk2C = data->unk03;
        state->unk30 = data->unk04;
        state->unk34 = data->unk05;
        state->unk38 = data->unk06;
        state->unk3C = data->unk07;

        i = 0;
        if (count > 0) {
            register u8 **slot __asm__("$3") = (u8 **)data;
            do {
                slot[3] = ptr;
                slot[5] = ptr;
                ptr += data->sizes[0] * 4;
                data = (LoadedData *)((u8 *)data + 2);
                i++;
                slot++;
            } while (i < count);
        }
        gStreamDescriptorPtr->end1 = ptr;
        gStreamDescriptorPtr->end2 = ptr;
    } else {
        __asm__("" : "+r"(data));
        gDemoPlayback = 0;
    }
}
