// Categories: Gameplay > State Initialization
#include "game/types.h"

struct UnkFA94 {
    u8 pad0[0xC];
    s32 unkC;
};

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern u8 gProfileMenuControllerMode[] __asm__("D_800A36BD");
extern struct UnkFA94 *gGameSession __asm__("D_8009FA94");

extern void gArmProfileTransfer(void) __asm__("func_8007FA88");
extern void func_8007F02C(void);

/**
 *  @brief Resets state fields and sets a flag based on the current mode.
 *
 *  Initializes fields in the global state block around two helper calls,
 *  then records whether the mode field differs from two.
 *  @par Globals
 *  D_8009F92C points to the state block; D_8009F8BD and D_8009F8BE are cleared.
 *  D_8009FA94 supplies the mode field; D_800A36BD receives the resulting flag.
 *  @par Control flow
 *  Calls the helpers in sequence, then branches on D_8009FA94->unkC.
 *  @par Calls
 *  func_8007FA88 runs after the first block updates; func_8007F02C runs
 *  after the remaining resets and before the mode-dependent flag is set.
 *  @par Side effects
 *  Writes state-block fields and global bytes, and invokes both helpers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8007F93C(void)
{
    u8 *p;

    gSaveProfileImage[0x93D] = 0;
    gSaveProfileImage[0x93F] = 0;
    gSaveProfileImage[0x940] = 1;
    gArmProfileTransfer();
    gSaveProfileImage[0x921] = 1;
    p = gSaveProfileImage;
    gUiHoldFlag = 0;
    gUiInteractionEnabled = 0;
    *(u16 *)(p + 0x942) = 0;
    *(u8 *)(p + 0x944) = 0;
    func_8007F02C();
    if (gGameSession->unkC == 2) {
        gProfileMenuControllerMode[0] = 0;
    } else {
        gProfileMenuControllerMode[0] = 1;
    }
}
