// Categories: Game Engine > Resource Cleanup
#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0xD];
    s8 owner;
    u8 unk0E[2];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
    s32 state;
    u8 unk0C[4];
    s32 unk10;
    u8 unk14[4];
    u8 flag18;
};

extern struct Unk4749CHandle gAssetHandle __asm__("D_800A0A80");

extern void func_80022240(s32 owner);
extern void func_80029720(void);
extern void func_80045370(void);
extern s32 func_800474F0(struct Unk4749CHandle *handle);

/**
 *  @brief Resets a global handle and performs associated cleanup.
 *
 *  When the handle is in state one, its resource owner is processed if
 *  present and an additional cleanup call depends on the handle flag.
 *  @par Globals
 *  D_800A0A80 supplies the handle, resource and flag; its state and unk10
 *  fields are cleared as applicable.
 *  @par Control flow
 *  State, resource and flag checks gate conditional calls; final cleanup is
 *  always performed after those checks.
 *  @par Calls
 *  func_80022240 processes the resource owner; func_80045370 performs
 *  flag-dependent cleanup; func_800474F0 processes the handle and
 *  func_80029720 performs the final cleanup step.
 *  @par Side effects
 *  Changes handle fields and invokes cleanup routines.
 *  @par Callers
 *  Confirmed: func_80023D78, gSoundHaltAllVoices.
 */
void func_80023E84(void)
{
    if (gAssetHandle.state == 1) {
        if (gAssetHandle.resource != 0) {
            func_80022240(gAssetHandle.resource->owner);
        }
        gAssetHandle.state = 0;
        if (gAssetHandle.flag18 == 0) {
            func_80045370();
        }
    }
    func_800474F0(&gAssetHandle);
    gAssetHandle.unk10 = 0;
    func_80029720();
}
