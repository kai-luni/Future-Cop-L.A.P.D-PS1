// Categories: Game Engine > Menu System | Input > Menu Navigation
#include "game/types.h"

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Unk8009FA94;

typedef struct {
    u8 pad00[0x24];
    void *unk24;
} Unk8009F468;

extern Unk8009F468 *gFontBankTable __asm__("D_8009F468");
extern u8 gRowClearFlag __asm__("D_8009FA10");
extern Unk8009FA94 *gGameSession __asm__("D_8009FA94");
extern u8 gMenuDescriptor[] __asm__("D_800A3808");
extern void gSelectEnabledMenuItem(s32, s32, s32) __asm__("func_800787D8");

/**
 * @brief Links the active front-end menu's parent to D_800A3808 and selects item 3 when gated.
 *
 * Confirmed: writes D_800A3808 (main menu descriptor) into D_8009F468->unk24 (+0x24 parent link), and if D_8009FA10 != 0 with (D_8009FA94->unk5C == 0 || D_8009FA94->unk0C == 1), calls func_800787D8(3, 1, 0) to pick the first enabled group-0 item from index 3.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F468 - active menu pointer (+0x24 written); D_800A3808 - parent menu address stored; D_8009FA10 - mode gate byte (read); D_8009FA94 - mission state (+0x0C, +0x5C read).
 *
 * @par Control flow
 * Unconditional store to D_8009F468->unk24 followed by nested guards on D_8009FA10 and D_8009FA94->unk5C/unk0C before func_800787D8.
 *
 * @par Calls
 * func_800787D8 - advances forward from index 3 in group 0 to select the first enabled item in D_8009F468.
 *
 * @par Side effects
 * Mutates D_8009F468->unk24 (+0x24) and conditionally updates D_8009F468[0] via func_800787D8.
 *
 * @par Callers
 * Confirmed: address taken by func_80077AD4 (0x80077C74 for D_800A3868/898/8C8 .unk10) and func_80078650 (0x800786F4, pointer test re-invoking D_8009F468->unk10) / Likely: none / Unknown: none.
 */
void gEnterGameHubMenu(void) __asm__("func_8007B3F4");

void gEnterGameHubMenu(void)
{
    Unk8009FA94 *g;

    gFontBankTable->unk24 = gMenuDescriptor;
    if (gRowClearFlag != 0) {
        g = gGameSession;
        if (g->unk5C == 0 || g->unk0C == 1) {
            gSelectEnabledMenuItem(3, 1, 0);
        }
    }
}
