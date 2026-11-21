// Categories: Game Engine > State Initialization
#include "game/types.h"

struct State {
    u8 unk00;
    u8 unk01;
    u8 unk02;
    s8 unk03;
    u8 unk04;
    u8 unk05;
    s16 unk06;
    u8 unk08;
    u8 unk09;
    u8 unk0A;
    u8 unk0B;
    void *unk0C;
    void *unk10;
    void *unk14;
    void *unk18;
    void *unk1C;
    void *unk20;
    void *unk24;
    void *unk28;
};

extern struct State gTitleScreenMenuDescriptor __asm__("D_800A38F8");
extern struct State gMenuDescriptor __asm__("D_800A3808");
extern struct State gOnePlayerMissionSelectMenuDescriptor __asm__("D_800A37A8");
extern struct State gTwoPlayerMissionSelectMenuDescriptor __asm__("D_800A37D8");
extern struct State D_800A3868;
extern struct State D_800A3898;
extern struct State D_800A38C8;
extern struct State D_800A3748;
extern struct State gAudioOptionsMenuDescriptor __asm__("D_800A3778");
extern struct State gOnePlayerLoadoutMenuDescriptor __asm__("D_800A3628");
extern struct State gTwoPlayerLoadoutMenuDescriptor __asm__("D_800A3658");
extern struct State gFrontEndTransitionMenuDescriptor __asm__("D_800A3688");
extern struct State D_800A35F8;
extern struct State gScreenDescriptor __asm__("D_800A3838");
extern struct State gProfileDetailMenuDescriptor __asm__("D_800A36E8");
extern struct State gProfileNameEntryMenuDescriptor __asm__("D_800A36B8");
extern struct State gMenuActivationBlock __asm__("D_800A3718");

extern u8 D_8009BED0[];
extern u8 gMainMenuItems[] __asm__("D_8009BBF8");
extern u8 D_8009BAE0[];
extern u8 gTwoPlayerMissionSelectMenuItems[] __asm__("D_8009BB34");
extern u8 D_8009BD10[];
extern u8 D_8009BDB8[];
extern u8 D_8009BE28[];
extern u8 D_8009BA1C[];
extern u8 gSoundOptionsMenuItems[] __asm__("D_8009BA38");
extern u8 D_8009B760[];
extern u8 gTwoPlayerLoadoutMenuItems[] __asm__("D_8009B7D0");
extern u8 D_8009B8B0[];
extern u8 D_8009B744[];
extern u8 D_8009BCBC[];
extern u8 D_8009B8CC[];
extern u8 gMemoryCardPromptMenuItems[] __asm__("D_8009B9E4");

extern u8 D_8007A2C4[];
extern u8 D_80079F54[];
extern u8 D_8007B17C[];
extern u8 D_8007B0D4[];
extern u8 D_8007B004[];
extern u8 D_8007AB0C[];
extern u8 D_8007BCF4[];
extern u8 D_8007B3F4[];
extern u8 gPauseMenuCancelCallback[] __asm__("D_8007F618");
extern u8 D_8007C088[];
extern u8 D_8007C6EC[];
extern u8 D_8007C93C[];
extern u8 gOptionsMenuInitCallback[] __asm__("D_8007D18C");
extern u8 D_8007CFA0[];
extern u8 D_8007CACC[];
extern u8 gExitOptionsSoundMenu[] __asm__("D_8007CA30");
extern u8 D_8007DD08[];
extern u8 D_8007D408[];
extern u8 D_8007E67C[];
extern u8 D_8007E7A8[];
extern u8 D_8007F5C8[];
extern u8 D_8007F678[];
extern u8 D_8007F8FC[];
extern u8 D_8007F93C[];
extern u8 D_80080038[];
extern u8 gGetProfileSlotEditState[] __asm__("D_8008228C");
extern u8 D_800821EC[];

extern s8 gOverlayDelay __asm__("D_8009F874");
extern u8 D_8009F875;
extern u8 gScreenUploadFlag __asm__("D_8009F876");
extern s32 gFontBankTable __asm__("D_8009F468");
extern s8 gPanelSyncValue __asm__("D_8009F8BC");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern u8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern u8 D_8009F8C1;
extern s32 gPendingRowCapture __asm__("D_8009F8C4");
extern s32 gDeferredTransition __asm__("D_8009F8C8");
extern s32 gIdleDeadline __asm__("D_8009F870");
extern s32 gFrameTickCounter[] __asm__("D_8009FA2C");

extern void gInitMenuDescriptor(struct State *state, u8 *name, s32 kind) __asm__("func_80077884");
extern void func_800778E4(void);
extern void func_80077990(void);
extern void gReleaseProfileCardOperation(void) __asm__("func_8007FACC");
extern void gResumeFrontEndForReturnMode(void) __asm__("func_800782EC");

/**
 *  @brief Registers the front-end menu descriptors and selects an initial screen.
 *
 *  The title descriptor D_800A38F8 uses D_8009BED0 and transitions through
 *  func_800778E4. The main descriptor gMenuDescriptor uses the seven 0x1C-byte
 *  entries at gMainMenuItems (D_8009BBF8). In the retail executable,
 *  entries 0/1 select one/two-player non-split mode via func_8007A3AC and
 *  func_8007A450; entries 3/4 select one/two-player split mode via
 *  func_8007A4CC and func_8007A56C. Entries 2/5 initially have enabled=0
 *  and use func_80079FE0/func_8007A0A4; func_8007A168 can enable them.
 *  Entry 6 links to D_800A36E8. Text IDs resolve through game resources.
 *  Descriptor +0x0C calls D_8007A2C4, which returns input unchanged and draws
 *  headings for player 0; +0x10 resets the menu with D_80079F54. Other
 *  descriptors have their own item tables and callbacks. gInitMenuDescriptor
 *  sets defaults before overrides.
 *  func_800782EC chooses the initial active panel based on the current mode;
 *  the default path activates the title, not the main menu directly.
 *  @par Globals
 *  D_800A35F8 through D_800A38F8 are the configured state descriptors.
 *  D_8009B744 through D_8009BED0 supply names; D_8007A2C4 and other
 *  D_8007xxxx and D_8008xxxx symbols supply stored handler or data pointers.
 *  D_8009F874, D_8009F875, D_8009F876, D_8009F468, D_8009F8BC,
 *  D_8009F8BD, D_8009F8BE, D_8009F8C0, D_8009F8C1, D_8009F8C4,
 *  and D_8009F8C8 are initialized; D_8009F870 uses D_8009FA2C[0].
 *  @par Control flow
 *  Performs a fixed sequence of registrations and assignments without loops or branches.
 *  @par Calls
 *  func_80077884 initializes each descriptor; func_800782EC performs further setup.
 *  @par Side effects
 *  Rewrites global descriptors and control values, and stores handler pointers.
 *  @par Callers
 *  Confirmed: func_800780DC.
 */
void gSetupFrontEndMenus(void) __asm__("func_80077AD4");

void gSetupFrontEndMenus(void)
{
    gInitMenuDescriptor(&gTitleScreenMenuDescriptor, D_8009BED0, 1);
    gTitleScreenMenuDescriptor.unk06 = 0;
    gTitleScreenMenuDescriptor.unk03 = 0;
    gTitleScreenMenuDescriptor.unk09 = 1;
    gTitleScreenMenuDescriptor.unk04 = 1;
    gTitleScreenMenuDescriptor.unk18 = func_800778E4;
    gTitleScreenMenuDescriptor.unk10 = func_80077990;
    gOverlayDelay = -1;
    D_8009F875 = 0;
    gScreenUploadFlag = 1;
    gInitMenuDescriptor(&gMenuDescriptor, gMainMenuItems, 7);
    gMenuDescriptor.unk0C = D_8007A2C4;
    gMenuDescriptor.unk06 = 0;
    gMenuDescriptor.unk03 = 1;
    gMenuDescriptor.unk08 = 1;
    gMenuDescriptor.unk10 = D_80079F54;
    gInitMenuDescriptor(&gOnePlayerMissionSelectMenuDescriptor, D_8009BAE0, 3);
    gOnePlayerMissionSelectMenuDescriptor.unk06 = 0;
    gOnePlayerMissionSelectMenuDescriptor.unk03 = 1;
    gOnePlayerMissionSelectMenuDescriptor.unk08 = 1;
    gOnePlayerMissionSelectMenuDescriptor.unk18 = D_8007B17C;
    gOnePlayerMissionSelectMenuDescriptor.unk05 = 1;
    gOnePlayerMissionSelectMenuDescriptor.unk1C = D_8007B0D4;
    gOnePlayerMissionSelectMenuDescriptor.unk10 = D_8007B004;
    gOnePlayerMissionSelectMenuDescriptor.unk0C = D_8007AB0C;
    gInitMenuDescriptor(&gTwoPlayerMissionSelectMenuDescriptor, gTwoPlayerMissionSelectMenuItems, 7);
    gTwoPlayerMissionSelectMenuDescriptor.unk06 = 0;
    gTwoPlayerMissionSelectMenuDescriptor.unk03 = 3;
    gTwoPlayerMissionSelectMenuDescriptor.unk08 = 1;
    gTwoPlayerMissionSelectMenuDescriptor.unk18 = D_8007B17C;
    gTwoPlayerMissionSelectMenuDescriptor.unk05 = 2;
    gTwoPlayerMissionSelectMenuDescriptor.unk1C = D_8007B0D4;
    gTwoPlayerMissionSelectMenuDescriptor.unk10 = D_8007B004;
    gTwoPlayerMissionSelectMenuDescriptor.unk0C = D_8007AB0C;
    gInitMenuDescriptor(&D_800A3868, D_8009BD10, 6);
    D_800A3868.unk06 = 0;
    D_800A3868.unk03 = 1;
    D_800A3868.unk08 = 1;
    D_800A3868.unk18 = D_8007BCF4;
    D_800A3868.unk10 = D_8007B3F4;
    D_800A3868.unk05 = 1;
    D_800A3868.unk20 = gPauseMenuCancelCallback;
    gInitMenuDescriptor(&D_800A3898, D_8009BDB8, 4);
    D_800A3898.unk06 = 0;
    D_800A3898.unk03 = 1;
    D_800A3898.unk08 = 1;
    D_800A3898.unk18 = D_8007BCF4;
    D_800A3898.unk10 = D_8007B3F4;
    D_800A3898.unk20 = gPauseMenuCancelCallback;
    gInitMenuDescriptor(&D_800A38C8, D_8009BE28, 6);
    D_800A38C8.unk06 = 0;
    D_800A38C8.unk03 = 1;
    D_800A38C8.unk08 = 1;
    D_800A38C8.unk18 = D_8007BCF4;
    D_800A38C8.unk10 = D_8007B3F4;
    D_800A38C8.unk20 = gPauseMenuCancelCallback;
    D_800A38C8.unk05 = 1;
    gInitMenuDescriptor(&D_800A3748, D_8009BA1C, 1);
    D_800A3748.unk10 = D_8007C088;
    D_800A3748.unk18 = D_8007C6EC;
    D_800A3748.unk06 = 0;
    D_800A3748.unk03 = 1;
    D_800A3748.unk08 = 1;
    D_800A3748.unk0C = D_8007C93C;
    gInitMenuDescriptor(&gAudioOptionsMenuDescriptor, gSoundOptionsMenuItems, 6);
    gAudioOptionsMenuDescriptor.unk10 = gOptionsMenuInitCallback;
    gAudioOptionsMenuDescriptor.unk18 = D_8007CFA0;
    gAudioOptionsMenuDescriptor.unk0C = D_8007CACC;
    gAudioOptionsMenuDescriptor.unk06 = 0;
    gAudioOptionsMenuDescriptor.unk03 = 1;
    gAudioOptionsMenuDescriptor.unk08 = 2;
    gAudioOptionsMenuDescriptor.unk14 = gExitOptionsSoundMenu;
    gInitMenuDescriptor(&gOnePlayerLoadoutMenuDescriptor, D_8009B760, 4);
    gOnePlayerLoadoutMenuDescriptor.unk06 = 0;
    gOnePlayerLoadoutMenuDescriptor.unk03 = 2;
    gOnePlayerLoadoutMenuDescriptor.unk08 = 1;
    gOnePlayerLoadoutMenuDescriptor.unk18 = D_8007DD08;
    gOnePlayerLoadoutMenuDescriptor.unk10 = D_8007D408;
    gOnePlayerLoadoutMenuDescriptor.unk05 = 1;
    gInitMenuDescriptor(&gTwoPlayerLoadoutMenuDescriptor, gTwoPlayerLoadoutMenuItems, 8);
    gTwoPlayerLoadoutMenuDescriptor.unk06 = 0;
    gTwoPlayerLoadoutMenuDescriptor.unk03 = 4;
    gTwoPlayerLoadoutMenuDescriptor.unk08 = 2;
    gTwoPlayerLoadoutMenuDescriptor.unk18 = D_8007DD08;
    gTwoPlayerLoadoutMenuDescriptor.unk10 = D_8007D408;
    gTwoPlayerLoadoutMenuDescriptor.unk05 = 2;
    gInitMenuDescriptor(&gFrontEndTransitionMenuDescriptor, D_8009B8B0, 1);
    gFrontEndTransitionMenuDescriptor.unk03 = 5;
    gFrontEndTransitionMenuDescriptor.unk06 = 0;
    gFrontEndTransitionMenuDescriptor.unk08 = 1;
    gFrontEndTransitionMenuDescriptor.unk18 = D_8007E67C;
    gInitMenuDescriptor(&D_800A35F8, D_8009B744, 1);
    D_800A35F8.unk10 = D_8007E7A8;
    D_800A35F8.unk06 = 0;
    D_800A35F8.unk03 = 1;
    D_800A35F8.unk08 = 1;
    D_800A35F8.unk1C = D_8007F5C8;
    gInitMenuDescriptor(&gScreenDescriptor, D_8009BCBC, 3);
    gScreenDescriptor.unk06 = 0;
    gScreenDescriptor.unk03 = 1;
    gScreenDescriptor.unk08 = 1;
    gScreenDescriptor.unk10 = D_8007F678;
    gInitMenuDescriptor(&gProfileDetailMenuDescriptor, D_8009B8CC, 0xA);
    gProfileDetailMenuDescriptor.unk10 = D_8007F8FC;
    gProfileDetailMenuDescriptor.unk14 = gReleaseProfileCardOperation;
    gProfileDetailMenuDescriptor.unk18 = D_80080038;
    gProfileDetailMenuDescriptor.unk06 = 0;
    gProfileDetailMenuDescriptor.unk0A = 0x57;
    gProfileDetailMenuDescriptor.unk0B = 0x35;
    gInitMenuDescriptor(&gProfileNameEntryMenuDescriptor, D_8009B8CC, 0xA);
    gProfileNameEntryMenuDescriptor.unk10 = D_8007F93C;
    gProfileNameEntryMenuDescriptor.unk14 = gReleaseProfileCardOperation;
    gProfileNameEntryMenuDescriptor.unk18 = D_80080038;
    gProfileNameEntryMenuDescriptor.unk06 = 0;
    gProfileNameEntryMenuDescriptor.unk0A = 0x57;
    gProfileNameEntryMenuDescriptor.unk0B = 0x35;
    gProfileNameEntryMenuDescriptor.unk1C = gGetProfileSlotEditState;
    gProfileNameEntryMenuDescriptor.unk0C = D_8007AB0C;
    gInitMenuDescriptor(&gMenuActivationBlock, gMemoryCardPromptMenuItems, 2);
    gMenuActivationBlock.unk14 = D_800821EC;
    gMenuActivationBlock.unk18 = D_80080038;
    gMenuActivationBlock.unk06 = 0;
    gFontBankTable = 0;
    gUiHoldFlag = 1;
    gUiInteractionEnabled = 1;
    gPlayerMenuDepth = 0;
    D_8009F8C1 = 0;
    gPendingRowCapture = 0;
    gDeferredTransition = -1;
    gPanelSyncValue = -1;
    gResumeFrontEndForReturnMode();
    gIdleDeadline = gFrameTickCounter[0] + 0x2328;
}
