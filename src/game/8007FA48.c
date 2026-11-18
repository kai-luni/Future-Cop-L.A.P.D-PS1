// Categories: Resources > Save Data | Resources > Memory Management
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void func_80020600(void);

/**
 * @brief Closes the profile save subsystem and releases both blocks allocated by
 *        func_8007F9CC().
 *
 * func_80020600() clears the memory-card manager pointer, then the manager block
 * at D_8009F92C + 0x904 and the 0x948-byte profile context are freed through
 * func_800181D4(). D_8009F92C keeps its stale value.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80076A2C (boot, before the intro movie is drawn) and the unit at
 * 0x80078530.
 */
void gShutdownProfileSubsystem(void) __asm__("func_8007FA48");

void gShutdownProfileSubsystem(void)
{
    func_80020600();
    gFreeBlock(*(void **)(gSaveProfileImage + 0x904));
    gFreeBlock(gSaveProfileImage);
}
