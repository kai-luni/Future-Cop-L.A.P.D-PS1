// Categories: Resources > Movie Playback | Game Engine > State Machine | Platform > MDEC
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 *gFontBankTable __asm__("D_8009F468");
extern s32 gStillRunningState __asm__("D_8009F464");
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern u8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern u8 D_800A36E8[];
extern u8 D_800A36B8[];
extern u8 D_800A3718[];
extern u8 D_800120A4[];

extern s32 func_800207B4();
extern void func_80078740(void);
extern s32 func_8001F37C(void);
extern void func_80080B70(void);
extern void func_8008204C(void);
extern void func_80081204(void);
extern s32 func_8007F750(void);
extern void func_80080010(void);
extern void func_80080AD4(void);
extern s32 func_80020C90(u8 *arg0);
extern s32 func_80020E68(s32 arg0);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_80078650(u8 *arg0, s32 arg1);
extern void func_80082020(void);

/**
 * @brief One step of the movie player: polls the decoder, tracks the state
 *        bytes and handles the end of the stream.
 *
 * @return The decoder status returned by func_800207B4(0): >= 0 is a normal
 *         stream status, negative values are the terminal states handled below.
 *
 * @par Confirmed structure
 * - The status is compared with the cached value at context + 0x208; when it is
 *   unchanged the whole body is skipped, so the work only runs on a status
 *   change.
 * - On a change the current state bytes 0x200 (state) and 0x201 (sub-state) are
 *   first copied to 0x203/0x204 ("previous"), a pending image buffer
 *   D_8009F468 is released when it is not one of the two static buffers, and
 *   the display flags D_8009F8BD/D_8009F8BE are armed.
 * - status >= 0: state becomes 2 and the sub-state 0. If the previous state was
 *   2 with a sub-state of 1..3 the transition depends on func_8001F37C()
 *   (present?) and on the countdown byte 0x922: it is decremented, or the
 *   sub-state becomes 3 when it reaches 0. Sub-state 1 with func_8007F750()
 *   success calls func_80080010() and, when the frame counter at 0x40C does not
 *   match, sets sub-state 3 and the constant 0x37D2 at 0x942; when the player
 *   state D_8009F464 equals 2 the movie is finished: func_80080AD4() runs and
 *   D_8009F464 = 3. Sub-state 2 writes 0x37A0 at 0x942.
 * - Sub-state 0 then looks for the next stream: func_80020C90(D_800120A4)
 *   (open file), else func_80020E68(0x678) (read 0x678 bytes); the latter
 *   success clears the 0x678-byte area at 0x20C, calls func_80080010() and
 *   clears D_8009F8BD when the counter at 0x921 is 0.
 * - status < 0 maps the terminal codes: default (0 and unknown) -> state 0,
 *   -1 -> state 3 with a marker in D_8009F468, -2 -> state 1, -4 -> state 2 with
 *   sub-state 1, -5 -> state 2 with sub-state 2 or 3 (depending on 0x207),
 *   -6 -> state 3 with sub-state 3 and 0x207 = 1. D_8009F8C0 is cleared and the
 *   status is cached at 0x208.
 *
 * @par Globals
 * D_8009F92C (movie context), D_8009F464 (player state), D_8009F468 (frame
 * buffer), D_8009F8BD/D_8009F8BE/D_8009F8C0 (display flags), D_800A36E8/
 * D_800A36B8 (static frame buffers), D_800A3718, D_800120A4 (file name).
 *
 * @par Likely meaning of the context bytes
 * 0x200 = player state, 0x201 = sub-state, 0x202 = next state, 0x203/0x204 =
 * previous state/sub-state, 0x205/0x206 = transition flags, 0x207 = "repeated
 * end" marker, 0x208 = cached decoder status, 0x921 = frames remaining,
 * 0x922 = countdown, 0x942 = last written value.
 *
 * @par Callers
 * func_80080B08 (one pump per boot-loop iteration) and func_80080038.
 */
s32 func_8007FB04(void)
{
    register s32 state __asm__("$18");
    register u32 s0 __asm__("$16");
    register u32 s1 __asm__("$17");

    state = func_800207B4(0);
    if (state != *(s32 *)(gSaveProfileImage + 0x208)) {
        if (gFontBankTable != 0 &&
            gFontBankTable != D_800A36E8 &&
            gFontBankTable != D_800A36B8) {
            func_80078740();
        }

        gSaveProfileImage[0x203] = gSaveProfileImage[0x200];
        gSaveProfileImage[0x204] = gSaveProfileImage[0x201];

        if (state >= 0) {
            gSaveProfileImage[0x200] = 2;
            gSaveProfileImage[0x201] = 0;
            gSaveProfileImage[0x202] = 0;
            s0 = 1;
            D_8009F8BD = s0;
            D_8009F8BE = s0;
            s1 = gSaveProfileImage[0x203];

            if (s1 == 2 && (u32)(gSaveProfileImage[0x204] - 1) < 3) {
                if (func_8001F37C() != 0) {
                    if (*(s8 *)(gSaveProfileImage + 0x922) != 0) {
                        gSaveProfileImage[0x922] = gSaveProfileImage[0x922] - 1;
                        if (gSaveProfileImage[0x204] == s1) {
                            func_80080B70();
                        } else if (gSaveProfileImage[0x204] == 3) {
                            func_8008204C();
                        } else {
                            func_80081204();
                        }
                    } else {
                        gSaveProfileImage[0x202] = 3;
                    }
                } else if ((u32)(gSaveProfileImage[0x204] - 1) < 2) {
                    gSaveProfileImage[0x202] = s1;
                    gSaveProfileImage[0x205] = s0;
                    s0 = gSaveProfileImage[0x204];
                    if (s0 == 1) {
                        if (func_8007F750() != 0) {
                            func_80080010();
                            if (*(s32 *)(gSaveProfileImage + 0x40C) != s0) {
                                gSaveProfileImage[0x205] = 3;
                                *(s16 *)(gSaveProfileImage + 0x942) = 0x37D2;
                            }
                            if (gStillRunningState == s1) {
                                func_80080AD4();
                                gStillRunningState = 3;
                            }
                        } else {
                            gSaveProfileImage[0x205] = s1;
                            D_8009F8BD = 0;
                        }
                    } else {
                        *(s16 *)(gSaveProfileImage + 0x942) = 0x37A0;
                    }
                }
            }

            if (gSaveProfileImage[0x202] == 0) {
                if (func_80020C90(D_800120A4) != 0) {
                    func_80081204();
                } else if (func_80020E68(0x678) == 0) {
                    D_8009F8BD = 0;
                    gSaveProfileImage[0x206] = 1;
                } else {
                    func_8001E4C0(gSaveProfileImage + 0x20C, 0, 0x678);
                    func_80080010();
                    if (gSaveProfileImage[0x921] == 0) {
                        D_8009F8BD = 0;
                    }
                }
            }
        } else {
            gSaveProfileImage[0x205] = 0;
            gSaveProfileImage[0x206] = 0;

            switch (state) {
            default:
                gSaveProfileImage[0x200] = 0;
                gSaveProfileImage[0x201] = 0;
                gSaveProfileImage[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 1;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -1:
                gSaveProfileImage[0x200] = 3;
                gSaveProfileImage[0x202] = 0;
                gSaveProfileImage[0x201] = 0;
                gSaveProfileImage[0x207] = 0;
                func_80078650(D_800A3718, 1);
                gFontBankTable[0] = 1;
                D_8009F8BD = 1;
                D_8009F8BE = 1;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -2:
                gSaveProfileImage[0x200] = 1;
                gSaveProfileImage[0x202] = 0;
                gSaveProfileImage[0x201] = 0;
                gSaveProfileImage[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -4:
                gSaveProfileImage[0x200] = 2;
                gSaveProfileImage[0x201] = 1;
                gSaveProfileImage[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                func_80082020();
                __asm__ volatile ("");
                break;
            case -5:
                gSaveProfileImage[0x200] = 2;
                if (gSaveProfileImage[0x207] != 0) {
                    gSaveProfileImage[0x201] = 3;
                } else {
                    gSaveProfileImage[0x201] = 2;
                }
                gSaveProfileImage[0x207] = 0;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                break;
            case -6:
                gSaveProfileImage[0x200] = 3;
                gSaveProfileImage[0x202] = 0;
                gSaveProfileImage[0x201] = 3;
                gSaveProfileImage[0x207] = 1;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                func_80082020();
                *(s16 *)(gSaveProfileImage + 0x942) = 0;
                break;
            }
            gPlayerMenuDepth = 0;
        }
        *(s32 *)(gSaveProfileImage + 0x208) = state;
    }
    return state;
}
