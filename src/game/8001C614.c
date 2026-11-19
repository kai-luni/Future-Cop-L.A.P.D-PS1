// Categories: Platform > CD-ROM | Resources > File Access
typedef unsigned char u8;
typedef int s32;

struct State {
    volatile s32 mode;
    char pad[24];
    s32 value;
};

extern struct State D_80010000;
extern s32 func_8008E194(s32 value);
void gCdSeekDoneCallback(u8 arg) __asm__("func_8001C614");


/**
 * @brief CD-ROM driver completion callback that advances D_80010000.mode and restores the saved handler.
 *
 * Confirmed: switches on event byte arg (ignoring 0, 1, 3, 4, 5); on case 2, if D_80010000.mode (+0x00) equals 1, sets it to 2 and calls func_8008E194(D_80010000.value) (+0x1C) to restore the previous D_8009C8A8 callback. Installed via func_8008E194 by CD-ROM loader routines func_8001C718 and func_8001CE54.
 *
 * @param arg Driver event byte (case 2 triggers completion when D_80010000.mode == 1).
 *
 * @par Globals
 * D_80010000 - CD-ROM loader state block at 0x80010000 (mode at +0x00 checked/written; saved handler at +0x1C read).
 *
 * @par Control flow
 * Jump-table switch over arg (0..5) with a single conditional D_80010000.mode == 1 guard inside case 2.
 *
 * @par Calls
 * func_8008E194 - restores the previous CD-ROM driver callback D_8009C8A8 from D_80010000.value (+0x1C).
 *
 * @par Side effects
 * Sets D_80010000.mode = 2 and overwrites D_8009C8A8 via func_8008E194.
 *
 * @par Callers
 * Confirmed: address taken by func_8001C718 (0x8001C7A8) and func_8001CE54 (0x8001D01C) for callback dispatch via D_8009C8A8 / Likely: none / Unknown: none.
 *
 * @par Role and path
 * CD-ROM driver event callback that ignores events 0, 1, 3, 4, and 5 and on
 * event 2, if D_80010000.mode == 1, sets D_80010000.mode = 2 and calls
 * func_8008E194(D_80010000.value) to restore the prior callback. Installed
 * via func_8008E194 by func_8001C718 and func_8001CE54.
 */
void gCdSeekDoneCallback(u8 arg)
{
    switch (arg) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
        break;
    case 2:
        if (D_80010000.mode == 1) {
            D_80010000.mode = 2;
            func_8008E194(D_80010000.value);
        }
        break;
    }
}
