// Categories: Game Engine > Object Setup > Registry | Game Engine > Game State > Init
#include "game/types.h"

struct Unk780DC {
    u32 unk0;
    s32 unk4;
};

extern struct Unk780DC gMenuUpActions[2] __asm__("D_800A3588");
extern struct Unk780DC gMenuDownActions[2] __asm__("D_800A3598");
extern struct Unk780DC gMenuLeftActions[2] __asm__("D_800A35A8");
extern struct Unk780DC gMenuRightActions[2] __asm__("D_800A35B8");
extern struct Unk780DC gMenuAcceptActions[2] __asm__("D_800A35C8");
extern struct Unk780DC gMenuBackActions[2] __asm__("D_800A35D8");
extern struct Unk780DC gMenuStartActions[2] __asm__("D_800A35E8");

extern u8 gArmedFlag __asm__("D_8009FAC4");
extern u8 gModuleLoadState __asm__("D_8009F3F0");
extern s32 gPendingModuleNameFlag __asm__("D_8009F3EC");
extern s32 gPublishedBlockPtr __asm__("D_8009F3F4");
extern s32 gPlayer1PrevMenuButtons __asm__("D_8009F8B0");
extern s32 gPlayer2PrevMenuButtons __asm__("D_8009F8B4");
extern s32 gDemoPlayback __asm__("D_8009FA84");

extern void gUpdatePrecinctUnlockFlag(void) __asm__("func_80077FFC");
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern void gFreeTextTable(void) __asm__("func_800898C8");
extern void gOpenProfileSaveSubsystem(void) __asm__("func_8007F9CC");
extern void gSetupFrontEndMenus(s32 arg) __asm__("func_80077AD4");
void gInitFrontEndSubsystems(void) __asm__("func_800780DC");


/**
 * Initialises the object registry and the per-slot flag tables of this subsystem.
 *
 * The registry is the 0x78 byte block (30 pointers) allocated from the tag-2 pool,
 * zeroed and published through D_8009F3F4. Its lifetime is fixed by two verified
 * neighbours: func_8008263C files an object into slot `obj->unk1C` (valid 1..0x1D,
 * i.e. 29 usable slots) and func_8007852C frees every occupied slot plus the array
 * on teardown. The block is therefore a 30-entry registry keyed by a type id.
 *
 * The seven 2-entry tables receive state 2 in unk4 and distinct controller bits
 * in unk0 (bits 19..23, 28 and 30). func_80078DD0 reads them for both players:
 * up, down, left, right, confirm, back and an additional confirm input.
 *
 * Called as the final step of func_80024904, the per-session setup that also sets
 * the view window, seeds the RNG and programs the display mode.
 *
 * @par Callers
 * func_80024904 only, as the final step of the per-session setup.
 *
 * @par Unknown
 * The exact role of func_80077FFC, func_800898C8 and func_8007F9CC in this
 * sequence.
 */
void gInitFrontEndSubsystems(void)
{
    void *p;
    u32 i;

    gArmedFlag = 0;
    gModuleLoadState = 1;
    gPendingModuleNameFlag = 0;
    gUpdatePrecinctUnlockFlag();
    p = gAllocMemory(0x78, 2);
    gPublishedBlockPtr = (s32)p;
    gFillMemory(p, 0, 0x78);
    gFreeTextTable();
    for (i = 0; i < 2; i++) {
        gMenuBackActions[i].unk4 = 2;
        gMenuAcceptActions[i].unk4 = 2;
        gMenuRightActions[i].unk4 = 2;
        gMenuLeftActions[i].unk4 = 2;
        gMenuDownActions[i].unk4 = 2;
        gMenuUpActions[i].unk4 = 2;
        gMenuStartActions[i].unk0 = 0x00080000;
        gMenuStartActions[i].unk4 = 2;
        gMenuUpActions[i].unk0 = 0x00100000;
        gMenuDownActions[i].unk0 = 0x00400000;
        gMenuLeftActions[i].unk0 = 0x00800000;
        gMenuRightActions[i].unk0 = 0x00200000;
        gMenuAcceptActions[i].unk0 = 0x40000000;
        gMenuBackActions[i].unk0 = 0x10000000;
    }
    gOpenProfileSaveSubsystem();
    gSetupFrontEndMenus(1);
    gPlayer1PrevMenuButtons = 0;
    gPlayer2PrevMenuButtons = 0;
    gDemoPlayback = 0;
}
