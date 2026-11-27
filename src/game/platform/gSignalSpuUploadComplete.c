// Categories: Platform > Audio | Game Engine > Request Queue
#include "game/types.h"

extern s32 gSpuUploadCompleted __asm__("D_8009F628");

struct Unk21824 {
    u32 f0;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fA;
    u16 fC;
    u16 fE;
    u16 f10;
    u16 f12;
    u32 f14;
    u32 f18;
    u16 f1C;
    u16 f1E;
    u32 f20;
    u32 f24;
};

void gSignalSpuUploadComplete(void) __asm__("func_80021888");

/**
 * @brief Signals completion of a sound bank upload that is being waited on.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the immediate one into $v0 and stores it into gSpuUploadCompleted,
 * then returns.
 *
 * @par Globals
 * Sets gSpuUploadCompleted to one.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * gUploadSoundBankToSpu installs this callback when it waits for a transfer
 * synchronously.
 *
 * @par Unknown
 * The callback does not inspect any request payload.
 */
void gSignalSpuUploadComplete(void)
{
    gSpuUploadCompleted = 1;
}
