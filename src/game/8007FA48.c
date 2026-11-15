// Categories: Resources > Movie Playback | Resources > Memory Management
#include "game/types.h"

extern u8 *D_8009F92C;
extern void func_800181D4(void *block);
extern void func_80020600(void);

/**
 * @brief Closes the movie player and releases both blocks allocated by
 *        func_8007F9CC().
 *
 * func_80020600() shuts the decoder down, then the work buffer at
 * D_8009F92C + 0x904 and the 0x948-byte context are freed through
 * func_800181D4(). D_8009F92C keeps its stale value.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80076A2C (after the intro movie) and the unit at 0x80078530.
 */
void func_8007FA48(void)
{
    func_80020600();
    func_800181D4(*(void **)(D_8009F92C + 0x904));
    func_800181D4(D_8009F92C);
}
