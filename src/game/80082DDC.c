// Categories: Graphics > Text > Debug Numbers | Game Engine > Frame
#include "game/types.h"

extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 D_8009F3D0;
extern s32 D_8009F870;
extern s8 D_8009F874;
extern u8 D_8009F4AC[];

extern void func_80047690(u8 enable);
extern void func_80089D44(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
extern s32 func_8008CD30(u8 *buffer, u8 *format, s32 value, s32 arg3);

/**
 * @brief Draws a timed debug text box in one of two layouts and accounts the
 *        elapsed time.
 *
 * @param arg0 Value formatted into the text.
 * @param arg1 Layout: 1 the small centred box, 0/2 the wide bottom box.
 * @return Nothing.
 *
 * @par Confirmed
 * Saves D_8009FA2C, disables via func_80047690(0), formats
 * func_8008CD30(buf, D_8009F4AC, D_8009F3D0, arg0) and draws through
 * func_80089D44: layout 1 at (0x26, 0x20, 0x31, 0x100, 0x90), layouts 0/2
 * at (4, 0, 0x18, 0x140, 0xC0). Re-enables, sets D_8009F874 = -1 and adds
 * the elapsed D_8009FA2C delta to D_8009F870.
 *
 * @par Likely role
 * A profiling/debug overlay of the front-end helpers func_80079F54,
 * func_80079FE0, func_8007A0A4: the elapsed-time accounting suggests a
 * frame-cost display. Note func_80089D44 ignores its width/height
 * arguments (measured in round 3).
 *
 * @par Unknown
 * The format string behind D_8009F4AC.
 *
 * @par Callers
 * func_80079F54, func_80079FE0, func_8007A0A4.
 */
void func_80082DDC(s32 arg0, s32 arg1)
{
    u8 buf[0x28];
    s32 start;

    start = gFrameTickCounter;
    func_80047690(0);
    switch (arg1) {
    case 1:
        func_8008CD30(buf, D_8009F4AC, D_8009F3D0, arg0);
        func_80089D44((s32)buf, 0x26, 0x20, 0x31, 0x100, 0x90);
        break;
    case 0:
    case 2:
        func_8008CD30(buf, D_8009F4AC, D_8009F3D0, arg0);
        func_80089D44((s32)buf, 4, 0, 0x18, 0x140, 0xC0);
        break;
    }
    func_80047690(1);
    D_8009F874 = -1;
    D_8009F870 += gFrameTickCounter - start;
}
