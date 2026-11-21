// Categories: Platform > Device Service | Platform > Audio
#include "game/types.h"

typedef struct {
    u8 pad0[0x408];
    volatile s32 unk408;
    volatile s32 unk40C;
    volatile s32 unk410;
    u8 pad414[0x8];
    volatile s32 unk41C;
    volatile s32 unk420;
} Unk8009F970;

extern Unk8009F970 *gMovieStreamContext __asm__("D_8009F970");
extern s32 D_8009F510;
extern s32 D_8009F514;
extern u8 gCodeDataSource[] __asm__("D_80088D74");
extern u8 gOffsetCdLocationBySectors[] __asm__("func_800889DC");

extern s32 gCdSync(s32 mode, u8 *result) __asm__("func_8008E154");
extern void gCdControlB(s32 mode, u8 *buf, s32 arg2) __asm__("func_8008E42C");
extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
extern s32 gSetCdSyncCallback(s32 value) __asm__("func_8008E194");
extern s32 gSetCdReadyCallback(s32 value) __asm__("func_8008E1A8");

/**
 * @brief Starts or stops a device service together with its two handles.
 *
 * With a non-zero argument the function releases mode 0 through func_8008E154,
 * sends the one-byte command block 0xA0 via func_8008E42C(0xE, ...), takes the
 * device lock, clears the three state words at 0x408/0x40C/0x410 and stores two
 * allocated handles in D_8009F510 and D_8009F514 (one from a data address, one
 * from func_800889DC + 0x40). The two run flags 0x420 and 0x41C are set to 1.
 * The stop path raises the lock, releases both handles, zeroes them and clears
 * the run flags. Both paths close with the matching unlock call.
 *
 * @param arg0 Non-zero starts the service, zero stops it.
 *
 * @par Globals
 * gMovieStreamContext - device state block; D_8009F510/D_8009F514 - handle pair;
 * D_80088D74 - data source; func_800889DC - code/data base used for the second handle.
 *
 * @par Control flow
 * Two symmetrical branches sharing the trailing unlock.
 *
 * @par Calls
 * func_8008E154 - set transfer mode. func_8008E42C - submode with buffer.
 * func_8008C9C0/func_8008C9E0 - lock and unlock. func_8008E194 - allocate handle.
 * func_8008E1A8 - allocate handle from a different pool.
 *
 * @par Side effects
 * Writes the state block and both handle globals.
 *
 * @par Callers
 * Likely: gMovieStreamReaderInit (undocumented sibling in the same address block).
 */
void gCdStreamService(s32 arg0) __asm__("func_800895E4");

void gCdStreamService(s32 arg0)
{
    u8 buf[8];

    if (arg0 != 0) {
        gCdSync(0, 0);
        buf[0] = 0xA0;
        gCdControlB(0xE, buf, 0);
        gDisableCpuInterrupts();
        gMovieStreamContext->unk410 = 0;
        gMovieStreamContext->unk408 = 0;
        gMovieStreamContext->unk40C = 0;
        D_8009F510 = gSetCdSyncCallback((s32)gCodeDataSource);
        D_8009F514 = gSetCdReadyCallback((s32)(gOffsetCdLocationBySectors + 0x40));
        gMovieStreamContext->unk420 = 1;
        gMovieStreamContext->unk41C = 1;
    } else {
        s32 active;

        gDisableCpuInterrupts();
        active = gMovieStreamContext->unk420;
        gMovieStreamContext->unk41C = 0;
        gMovieStreamContext->unk410 = 0;
        gMovieStreamContext->unk408 = 0;
        gMovieStreamContext->unk40C = 0;
        if (active != 0) {
            gSetCdSyncCallback(D_8009F510);
            gSetCdReadyCallback(D_8009F514);
            D_8009F510 = 0;
            D_8009F514 = 0;
            gMovieStreamContext->unk420 = 0;
        }
    }
    gEnableCpuInterrupts();
}
