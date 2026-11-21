// Categories: Game Engine > Front-End Main Loop | Resources | Graphics | Input | Platform
#include "game/types.h"

typedef struct {
    u8 pad00[0x8];
    s32 unk8;
    s32 unkC;
    s32 unk10;
    u8 pad14[0x14];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[0xC];
    s32 unk4C;
    u8 pad50[0xC];
    s32 unk5C;
    u8 pad60[0x2C];
    s32 unk8C;
    s32 unk90;
    s32 unk94;
    s32 unk98;
} State;

extern State *gGameSession __asm__("D_8009FA94");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 gPendingScreenState __asm__("D_8009F9A8");
extern s32 gSavedPlayer1PrimaryWeaponId __asm__("D_8009F850");
extern s32 gSavedPlayer1HeavyWeaponId __asm__("D_8009F854");
extern s32 gSavedPlayer1SpecialWeaponId __asm__("D_8009F858");
extern s32 gSavedPlayer2PrimaryWeaponId __asm__("D_8009F85C");
extern s32 gSavedPlayer2HeavyWeaponId __asm__("D_8009F860");
extern s32 gSavedPlayer2SpecialWeaponId __asm__("D_8009F864");
extern s8 gSavedPlayer1DifficultyLevel __asm__("D_8009F868");
extern s8 gSavedPlayer2DifficultyLevel __asm__("D_8009F869");
extern u8 gUploadRequestFlag __asm__("D_8009F3E5");
extern u8 gModuleLoadState __asm__("D_8009F3F0");
extern u8 gMovieAbortFlag __asm__("D_8009F4E0");
extern s32 gPendingModuleNameFlag __asm__("D_8009F3EC");
extern s32 gMoviePathPrefix __asm__("D_8009F3D0");
extern u8 gConcatStringsFormat[] __asm__("D_8009F3A0");
extern u8 gFrontEndBootstrapModule[] __asm__("D_8009F3A8");
extern u8 gMissionModuleSuffix[] __asm__("D_8009F3B0");
extern u8 gFrontEndModuleName[] __asm__("D_8009B300");
extern u8 gEaLogoName[] __asm__("D_80011E3C");
extern u8 gIntroMovieFilename[] __asm__("D_80011E48");

extern void func_800965C8(void);
extern u8 gInitRuntime(void) __asm__("func_80076EB0");
extern void gShutdownRuntime(void) __asm__("func_80076FE0");
extern void func_8007BFCC(s32, s32);
extern void func_80077050(void);
extern void func_8001B2C8(s32, s32);
extern void gShowLoadingScreen(void) __asm__("func_80025370");
extern void func_8007F9CC(void);
extern void gBeginBootProfileTransfer(void) __asm__("func_8007F8BC");
extern void gArmProfileTransfer(void) __asm__("func_8007FA88");
extern void gResetProfileTransferFlags(void) __asm__("func_8007F8E0");
extern void gStepBootProfileTransfer(void) __asm__("func_80080B08");
extern s32 gIsBootProfileTransferDone(void) __asm__("func_8007F8CC");
extern void gReleaseProfileCardOperation(void) __asm__("func_8007FACC");
extern void gShutdownProfileSubsystem(void) __asm__("func_8007FA48");
extern void gDrawImageResource(void *, s32, s32, s32, s32, s32) __asm__("func_80089D44");
extern void func_8008CD30(void *, void *, s32, s32);
extern void func_8001B448(void);
extern void func_80077088(void);
extern void func_800780C0(s32);
extern void gRunScreenModule(void *, s32) __asm__("func_800287F4");
extern void func_8008CC90(void *, void *);
extern void func_8008CCB0(void *, s32);
extern s32 func_80083130(void *);

/**
 * @brief Front-end main loop of the game: boots the runtime, loads the
 *        front-end module and then runs the screen loop for the rest of the
 *        program's life.
 *
 * Confirmed role
 * --------------
 * The PS-X EXE header of SLUS_007.39 names @c 0x80096520 as PC0; that stub
 * (entrypoint) clears .bss, computes the heap range into D_8009EA04/D_8009EA08,
 * installs $gp and then @c jal func_80076A2C. This function is therefore the
 * logical @c main() and the caller of everything else, but it is not the
 * technical entry point.
 *
 * Sequence (confirmed by the code)
 * -------------------------------
 * 1. func_800965C8() - one-shot initialiser with a guard flag in D_8009EA00;
 *    its constructor loop runs from func_80010000 with count 0, so in the
 *    retail build it only sets the guard.
 * 2. gInitRuntime() - device/state initialisation. A non-zero result means
 *    "runtime up", 0 skips the whole loop.
 * 3. The 0xF4-byte state block D_8009FA94 is initialised: unk28/unk2C/unk34/
 *    unk38 = 0/0x10/0/0x10 and unk30/unk3C = 0x20 (a 32x16 rectangle),
 *    unk8/unk98 = 1 (two "enabled" bytes), unk8C/unk90/unk94 and unk4C = 0.
 * 4. func_8007BFCC(0,0) and func_8007BFCC(1,0) - two display/buffer set-up
 *    calls, one per buffer index.
 * 5. D_8009F9A8 = unk10 = 3 (initial screen state), func_80077050()
 *    (reset the three fixed memory pools), func_8001B2C8(0xC8, 0x200)
 *    (allocate the render arena for 200 packets and 512 table words).
 * 6. D_8009F3E5 = 0, D_8009F3F0 = 0x14: the legal notice screen
 *    (D_8009B42C[20] == "alegal.lsc") is pre-selected.
 * 7. func_80025370() serves the loading screen, func_8007F9CC/8007F8BC/
 *    func_8007FA88/8007F8E0 arm the movie/graphics layer.
 * 8. Busy wait until D_8009FA2C has advanced by 0x5DC (~timing ticks) *and*
 *    gIsBootProfileTransferDone() reports completion; gStepBootProfileTransfer() is pumped in between
 *    (this is the only place the delay loop body appears).
 * 9. gReleaseProfileCardOperation(), gShutdownProfileSubsystem(), then two gDrawImageResource image draws:
 *    D_80011E3C ("Ea_logo.fsv") at (0,0) with mode 4, and the name built by
 *    func_8008CD30(local, "%s%s", D_8009F3D0, "Intro.wve") where D_8009F3D0
 *    points at the path prefix "Movies\\"
 *    at (0,0x18) with mode 4. The two trailing arguments of those calls
 *    (0x140/0xF0 and 0x140/0xC0) are width/height values that gDrawImageResource
 *    does not read - the rebuild is byte-exact without them.
 * 10. func_8001B448() and func_80077088() release the boot-time allocations,
 *     func_800780C0() installs the character table, and the infinite loop
 *     starts.
 *
 * Steady-state loop
 * -----------------
 * - First pass only (s2 == 1): gRunScreenModule("FE.mis", 1) starts the
 *   front-end module, then strcat(D_8009B300, ".mis") completes the module
 *   name buffer; afterwards the loop runs with that name.
 * - The six geometry words of the state block are shadowed into
 *   D_8009F850..D_8009F864 plus the two flags D_8009F868/D_8009F869 before
 *   gRunScreenModule(D_8009B300, 0) and copied back afterwards. The screen
 *   module therefore works on copies, and the loop keeps the authoritative
 *   values.
 * - The session setup path func_80024904 -> func_800780DC calls
 *   gSetupFrontEndMenus (func_80077AD4). The descriptor setup and menu
 *   dispatcher in src/game/80077AD4.c and src/game/80078DD0.c describe the
 *   menu itself; this loop loads modules rather than drawing items directly.
 * - If unk4C (selection field, compared to 10) equals 10 and unk5C/unkC equal 1,
 *   unk10 is forced to 0; the precise meaning of this special case is unknown.
 * - switch on unk10: case 0 handles a pending D_8009F3EC (copy the name with
 *   strcpy, strcat ".mis", switch to state 3 and set D_8009F3F0 = 5) and falls
 *   through to case 3, which re-arms the screen with D_8009F3F0 = 1 and s2 = 1
 *   (rerun the first-pass module load).
 * - While D_8009F9A8 != 0 the loop restarts (a screen is still settling).
 * - func_80083130(local + 0x50) returns a count plus a second word in
 *   local + 0x50; both zero means "nothing to do this frame". Otherwise the
 *   pool is re-armed (func_80077050) and up to two 320x192 blits are emitted
 *   from the buffer at local + 0x28, then func_80077088() releases the pool.
 *
 * @return Always 0; the function only returns after gInitRuntime() failed
 *         (runtime could not start) or after gShutdownRuntime() shut the runtime
 *         down, which the retail code never reaches because of the for(;;).
 *
 * @par Globals read/written
 * - D_8009FA94: 0xF4-byte global state block (screen state, cursor, rectangle,
 *   flags). Written during boot and copied in/out every frame.
 * - D_8009F850..D_8009F864 / D_8009F868 / D_8009F869: shadow copies of the
 *   state block used by the screen module.
 * - D_8009F9A8: mirror of unk10, the active screen state.
 * - D_8009FA2C: time base used for the boot wait.
 * - D_8009F3E5 / D_8009F3F0 / D_8009F3EC / gMovieAbortFlag: loading-screen request
 *   flag, screen index (see D_8009B42C), pending module name and an "inhibit
 *   overlay" flag.
 * - D_8009F3A8 ("FE.mis"), D_8009F3B0 (".mis"), D_8009F3A0 ("%s%s"),
 *   D_8009F3D0/8009F3D8/8009F3E0 (path prefixes), D_8009B300 (module name
 *   buffer), D_8009B2DC (table passed to func_8001D248).
 *
 * @par Side effects
 * Allocates and frees the render arena twice per run, initialises the device
 * layer, starts the front-end module and never returns to the caller (the
 * entrypoint's following @c break 0,1 is therefore unreachable in practice).
 *
 * @par Matching note
 * The @c register pins ($4/$5/$6 in the boot branch) and the "ten" temporary
 * exist purely to reproduce the retail register allocation; changing them
 * breaks the byte match without changing behaviour.
 *
 * @par Unknown
 * - What func_80083130() returns in local + 0x50 and what the two blits show.
 * - The exact meaning of unk5C and unkC (both compared against literal 1).
 */
s32 gGameMain(void) __asm__("func_80076A2C");

s32 gGameMain(void)
{
    u8 local[0x54];
    s32 s0;
    s32 s3;
    u8 s2;

    func_800965C8();
    s2 = 1;
    if (gInitRuntime() != 0) {
        register s32 z0 __asm__("$4") = 0;
        register s32 z1 __asm__("$5") = z0;
        register s32 ten __asm__("$6");
        __asm__ volatile("" : : "r"(z0), "r"(z1));
        ten = 0x10;
        gGameSession->unk30 = 0x20;
        gGameSession->unk3C = 0x20;
        gGameSession->unk28 = 0;
        gGameSession->unk2C = ten;
        gGameSession->unk34 = 0;
        gGameSession->unk38 = ten;
        gGameSession->unk8C = 0;
        gGameSession->unk90 = 0;
        gGameSession->unk94 = 0;
        gGameSession->unk8 = 1;
        gGameSession->unk98 = 1;
        gGameSession->unk4C = 0;

        func_8007BFCC(z0, z1);
        func_8007BFCC(1, 0);

        gPendingScreenState = 3;
        gGameSession->unk10 = 3;
        func_80077050();

        func_8001B2C8(0xC8, 0x200);

        gUploadRequestFlag = 0;
        gModuleLoadState = 0x14;

        gShowLoadingScreen();
        func_8007F9CC();
        gBeginBootProfileTransfer();
        gArmProfileTransfer();
        gResetProfileTransferFlags();

        s0 = gFrameTickCounter + 0x5DC;
        while (gFrameTickCounter < s0 || gIsBootProfileTransferDone() == 0) {
            gStepBootProfileTransfer();
        }

        s0 = 0x140;
        gReleaseProfileCardOperation();
        s3 = 1;
        gShutdownProfileSubsystem();

        gDrawImageResource(gEaLogoName, 4, 0, 0, s0, 0xF0);

        func_8008CD30(local, gConcatStringsFormat, gMoviePathPrefix, (s32)gIntroMovieFilename);
        gDrawImageResource(local, 4, 0, 0x18, s0, 0xC0);

        func_8001B448();
        func_80077088();
        func_800780C0(0);

        for (;;) {
            if (s2) {
                gRunScreenModule(gFrontEndBootstrapModule, 1);
                func_8008CC90(gFrontEndModuleName, gMissionModuleSuffix);
                s2 = 0;
                continue;
            }

            gSavedPlayer1PrimaryWeaponId = gGameSession->unk28;
            gSavedPlayer1HeavyWeaponId = gGameSession->unk2C;
            gSavedPlayer1SpecialWeaponId = gGameSession->unk30;
            gSavedPlayer2PrimaryWeaponId = gGameSession->unk34;
            gSavedPlayer2HeavyWeaponId = gGameSession->unk38;
            gSavedPlayer2SpecialWeaponId = gGameSession->unk3C;
            gSavedPlayer1DifficultyLevel = ((u8 *)gGameSession)[8];
            gSavedPlayer2DifficultyLevel = ((u8 *)gGameSession)[0x98];

            gRunScreenModule(gFrontEndModuleName, 0);

            gGameSession->unk28 = gSavedPlayer1PrimaryWeaponId;
            gGameSession->unk2C = gSavedPlayer1HeavyWeaponId;
            gGameSession->unk30 = gSavedPlayer1SpecialWeaponId;
            gGameSession->unk34 = gSavedPlayer2PrimaryWeaponId;
            gGameSession->unk38 = gSavedPlayer2HeavyWeaponId;
            gGameSession->unk3C = gSavedPlayer2SpecialWeaponId;
            ((s32 *)gGameSession)[2] = gSavedPlayer1DifficultyLevel;
            ((s32 *)gGameSession)[0x26] = gSavedPlayer2DifficultyLevel;

            if (gGameSession->unk4C == 10 &&
                gGameSession->unk5C == s3 &&
                gGameSession->unkC == s3) {
                gGameSession->unk10 = 0;
            }

            gPendingScreenState = gGameSession->unk10;
            switch (gGameSession->unk10) {
            case 0:
                if (gPendingModuleNameFlag != 0) {
                    func_8008CCB0(gFrontEndModuleName, gPendingModuleNameFlag);
                    func_8008CC90(gFrontEndModuleName, gMissionModuleSuffix);
                    gPendingScreenState = 3;
                    gPendingModuleNameFlag = 0;
                    gModuleLoadState = 5;
                    break;
                }
            case 3:
                gModuleLoadState = s3;
                s2 = 1;
            }

            if (gPendingScreenState != 0) {
                continue;
            }

            s0 = func_80083130(local + 0x50);
            if (s0 == 0 && *(s32 *)(local + 0x50) == 0) {
                continue;
            }

            func_80077050();

            if (s0 != 0) {
                func_8008CD30(local + 0x28, gConcatStringsFormat, gMoviePathPrefix, s0);
                gDrawImageResource(local + 0x28, 4, 0, 0x18, 0x140, 0xC0);
            }

            if (gMovieAbortFlag == 0 && *(s32 *)(local + 0x50) != 0) {
                func_8008CD30(local + 0x28, gConcatStringsFormat, gMoviePathPrefix, *(s32 *)(local + 0x50));
                gDrawImageResource(local + 0x28, 4, 0, 0x18, 0x140, 0xC0);
            }

            func_80077088();
        }
    }

    gShutdownRuntime();
    return 0;
}
