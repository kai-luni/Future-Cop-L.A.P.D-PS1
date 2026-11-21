// Categories: Game Engine > Gameplay Frame Loop | Graphics > Rendering | Physics > Camera
#include "game/types.h"

extern volatile s32 gGameClock __asm__("D_8009FA60");
extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gSceneFrameCounter __asm__("D_8009FA50");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern u8 gFrameDeltaNewest __asm__("D_8009F690");
extern u8 gFrameDeltaPrev1 __asm__("D_8009F691");
extern u8 gFrameDeltaPrev2 __asm__("D_8009F692");
extern u8 gFrameDeltaRemainder __asm__("D_8009F693");
extern u8 gFrameBarrier __asm__("D_8009FA3C");
extern void *gCamera __asm__("D_8009F9E4");

/* Incomplete array types keep these out of small data on the MIPS backend,
   which is why the oracle addresses them with lui/lw instead of ($gp). */
extern u8 *gFocusPositions[] __asm__("D_8009F9A0");
extern u8 *gPlayerEntities[] __asm__("D_8009FA58");

extern void gSetDisplayEnabled(u8 value) __asm__("func_8001BFD8");
extern void func_8002703C(void);
extern void gRunSoloSessionFrame(void) __asm__("func_800271A4");
extern void gRunSessionFrame(void) __asm__("func_80027510");
extern void func_80028164(void);
extern void func_80027D9C(void);
extern void func_80027D70(void);
extern void func_8001B4A8(s32 arg);
extern void func_800451EC(s32 value);
extern void gUpdateSoundVoiceAssignments(void) __asm__("func_800462D0");
extern void func_8008352C(s32 arg);
extern void gSelectViewRecord(s32 id) __asm__("func_80017B60");
extern void gSetPositionAngleEntry(s32 *position, s32 arg1, s32 angle, s32 index) __asm__("func_80045884");
void gRunGameFrameLoop(void) __asm__("func_80028618");


/**
 * @brief The in-game frame loop: input, update, camera matrix per player,
 *        world render and frame submission - repeated while the state is
 *        4 (playing), 8 or 0xD.
 *
 * @return Nothing; it returns when the state leaves those values.
 *
 * @par Confirmed flow
 * 1. Frame bookkeeping: D_8009FA60 = 0, frame id D_8009F9FC = 8, D_8009FA50 = 0,
 *    the three bytes D_8009F690..D_8009F692 are set to 8, D_8009F693 = 0 and
 *    D_8009FAA4/D_8009FA30 are seeded (D_8009FA30 = 0 - 8).
 * 2. If the state is not 4, 0xD or 8 the loop is skipped entirely.
 * 3. Per round: increment D_8009FA50, func_8002703C() (input/controller),
 *    then func_800271A4() when D_8009FA0C == 1 or func_80027510() otherwise
 *    (the two update paths - the same switch the rest of the renderer uses),
 *    and func_8008352C(frame id).
 * 4. For up to two entries of D_8009FA58 (the player/split-screen slots):
 *    func_80017B60() selects a bank from the entry's node, then
 *    func_80045884(position, camera + 0x7C, camera->0x20, index) builds the
 *    view matrix - the position pointer comes either from D_8009F9A0[0] + 0x50
 *    (when set) or from the entry itself at +0x50.
 * 5. func_800451EC(work), func_800462D0() render the world; when the state has
 *    become 0xD, func_80028164() plus func_8001B4A8(1) submit the frame.
 * 6. func_80027D70() always runs; when the state is still 4 or 0xD,
 *    func_80027D9C() runs and the frame barrier D_8009FA3C is raised, except
 *    when the state just changed to 0xD. In every other case the barrier is
 *    raised directly.
 * 7. The loop repeats while the state is 4, 0xD or 8; func_8001BFD8(1)
 *    re-enables the display on exit.
 *
 * @par Globals
 * D_8009FA00 (game state), D_8009FA0C (render/update path selector),
 * D_8009F9FC (frame id), D_8009FAA4/D_8009FA30 (frame timing), D_8009FA50
 * (frame counter), D_8009FA60 (cleared), D_8009F690..D_8009F693 (frame/state
 * bytes), D_8009FA3C (barrier), D_8009F9A0/D_8009FA58 (player slots),
 * D_8009F9E4 (camera object).
 *
 * @par Likely role
 * This is the "playing" loop: one call per displayed frame, with two players
 * supported (split screen or co-op) and the camera driven from a camera object
 * at D_8009F9E4. State 0xD appears to be the "leaving the level" frame in
 * which the last picture is submitted.
 *
 * @par Callers
 * func_800287F4, on the path where arg1 == 0 (game mode).
 *
 * @par Environment path
 * The in-game frame loop: it clears the game clock, sets the frame delta to
 * 8, resets gSceneFrameCounter and seeds the four frame-delta history bytes,
 * then repeats one iteration while gScreenState is 4, 8 or 0xD. Each
 * iteration advances gSceneFrameCounter, samples timing and input through
 * func_8002703C, runs the one-player update func_800271A4 or the two-player
 * gRunSessionFrame (func_80027510), calls func_8008352C with the frame delta
 * and, for each occupied slot of D_8009FA58, selects the slot's view record
 * with gSelectViewRecord and calls func_80045884(position, gCamera + 0x7C,
 * gCamera angle, slot), using the focus entity's +0x50 position instead of
 * the player's when D_8009F9A0 is set. It then calls func_800451EC and
 * func_800462D0, on state 0xD submits the last frame through func_80028164
 * and func_8001B4A8(1), finishes with func_80027D70/func_80027D9C and raises
 * gFrameBarrier; its only caller is gRunScreenModule (func_800287F4).
 */
void gRunGameFrameLoop(void)
{
    register s32 saved __asm__("$3");
    register u32 b __asm__("$2");
    register s32 work __asm__("$16");
    register u8 **cursor __asm__("$17");

    gGameClock = 0;
    saved = gGameClock;
    work = 8;
    gFrameDelta = work;
    b = *(u8 *)&gFrameDelta;
    gSceneFrameCounter = 0;
    gFrameDeltaRemainder = 0;
    gFrameTime = saved;
    gFrameTickBase = saved - 8;
    gFrameDeltaNewest = b;
    gFrameDeltaPrev1 = b;
    gFrameDeltaPrev2 = b;

    gSetDisplayEnabled(0);

    if (gScreenState != 4 && gScreenState != 0xD && gScreenState != work) {
        goto finished;
    }

frame:
    {
        gSceneFrameCounter = gSceneFrameCounter + 1;
        func_8002703C();
        if (gPlayerCount == 1) {
            gRunSoloSessionFrame();
        } else {
            gRunSessionFrame();
        }
        {
            register s32 frame_id __asm__("$4");
            frame_id = gFrameDelta;
            cursor = gPlayerEntities;
            work = 0;
            func_8008352C(frame_id);
        }

        do {
            register u8 *position __asm__("$4");
            register u8 *camera __asm__("$5");
            register s32 angle __asm__("$6");

            if (*cursor == 0) {
                break;
            }
            gSelectViewRecord((s32)*(s8 *)(*(u8 **)(*cursor + 0xAC) + 0xAC));
            position = gFocusPositions[0];
            if (position != 0) {
                position += 0x50;
                camera = (u8 *)gCamera;
                angle = *(s32 *)(camera + 0x20);
                camera += 0x7C;
                gSetPositionAngleEntry((s32 *)position, (s32)camera, angle, work);
            } else {
                position = *cursor + 0x50;
                camera = (u8 *)gCamera;
                angle = *(s32 *)(camera + 0x20);
                camera += 0x7C;
                gSetPositionAngleEntry((s32 *)position, (s32)camera, angle, work);
            }
            work++;
            cursor++;
        } while (work < 2);

        func_800451EC(work);
        work = 0xD;
        gUpdateSoundVoiceAssignments();

        if (gScreenState == work) {
            func_80028164();
            func_8001B4A8(1);
        }

        func_80027D70();

        if (gScreenState == 4 || gScreenState == work) {
            func_80027D9C();
            if (gScreenState != work) {
                gFrameBarrier = 1;
            }
        } else {
            gFrameBarrier = 1;
        }
    }

    if (gScreenState == 4 || gScreenState == 0xD || gScreenState == 8) {
        goto frame;
    }
finished:
    gSetDisplayEnabled(1);
}
