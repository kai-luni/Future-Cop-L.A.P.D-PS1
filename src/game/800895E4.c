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

extern Unk8009F970 *gQueueBlock __asm__("D_8009F970");
extern s32 D_8009F510;
extern s32 D_8009F514;
extern u8 D_80088D74[];
extern u8 func_800889DC[];

extern s32 func_8008E154(s32 mode, u8 *result);
extern void func_8008E42C(s32 mode, u8 *buf, s32 arg2);
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern s32 func_8008E194(s32 value);
extern s32 func_8008E1A8(s32 value);

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
 * D_8009F970 - device state block; D_8009F510/D_8009F514 - handle pair;
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
 * Likely: func_80089700 (undocumented sibling in the same address block).
 */
void func_800895E4(s32 arg0)
{
    u8 buf[8];

    if (arg0 != 0) {
        func_8008E154(0, 0);
        buf[0] = 0xA0;
        func_8008E42C(0xE, buf, 0);
        func_8008C9C0();
        gQueueBlock->unk410 = 0;
        gQueueBlock->unk408 = 0;
        gQueueBlock->unk40C = 0;
        D_8009F510 = func_8008E194((s32)D_80088D74);
        D_8009F514 = func_8008E1A8((s32)(func_800889DC + 0x40));
        gQueueBlock->unk420 = 1;
        gQueueBlock->unk41C = 1;
    } else {
        s32 active;

        func_8008C9C0();
        active = gQueueBlock->unk420;
        gQueueBlock->unk41C = 0;
        gQueueBlock->unk410 = 0;
        gQueueBlock->unk408 = 0;
        gQueueBlock->unk40C = 0;
        if (active != 0) {
            func_8008E194(D_8009F510);
            func_8008E1A8(D_8009F514);
            D_8009F510 = 0;
            D_8009F514 = 0;
            gQueueBlock->unk420 = 0;
        }
    }
    func_8008C9E0();
}
