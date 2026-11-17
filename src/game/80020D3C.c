// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

extern u32 *gOperationState __asm__("D_8009AB08");
extern u8 D_80010B0C[];

extern s32 func_80020C90(void *arg0);
extern void func_8008CD30(void *buf, void *fmt, u32 n, void *arg);
extern void func_8001F1B8(s32 a, s32 b, s32 c, s32 d);
extern s32 func_8001F37C(void);

/**
 * @brief Queues an asynchronous memory-card read for a named save file.
 *
 * Looks up the file slot via func_80020C90, formats the debug card path "bu0%1ld:%s"
 * using D_80010B0C into a stack buffer, rounds byte count arg2 up to 128-byte sectors,
 * and submits the read request through func_8001F1B8.
 *
 * @param arg0 Null-terminated save filename string.
 * @param arg1 Destination RAM address for the read data.
 * @param arg2 Number of bytes to read (0 returns 0 immediately).
 * @return Rounded 128-byte sector count n if the card manager accepted the request (unk9A4 == 0), else 0.
 *
 * @par Globals
 * D_8009AB08 - card manager whose first word (*D_8009AB08 >> 4) gives the port/channel; D_80010B0C - format string "bu0%1ld:%s".
 *
 * @par Control flow
 * Guards on arg2 != 0 and func_80020C90(arg0) != 0, queues func_8001F1B8(r << 6, n, arg1, 0), and returns n only if func_8001F37C() == 0.
 *
 * @par Calls
 * func_80020C90 - locates save slot; func_8008CD30 - formats unused path string; func_8001F1B8 - queues sector read; func_8001F37C - checks busy flag unk9A4.
 *
 * @par Side effects
 * Updates D_8009AB08 read request state (unk8F8 = 2, unk998/99C/910/90C/994/9A4) via func_8001F1B8.
 *
 * @par Callers
 * Confirmed: func_80081204 (0x80081244, loads 0x678-byte save profile image into D_8009F92C + 0x20C).
 */
s32 func_80020D3C(void *arg0, s32 arg1, u32 arg2)
{
    u8 buf[0x100];
    s32 ret;
    s32 r;
    s32 n;

    ret = 0;
    if (arg2 != 0) {
        r = func_80020C90(arg0);
        if (r != 0) {
            func_8008CD30(buf, D_80010B0C, *gOperationState >> 4, arg0);
            n = (arg2 + 0x7F) >> 7;
            func_8001F1B8(r << 6, n, arg1, ret);
            if (func_8001F37C() == 0) {
                ret = n;
            }
        }
    }
    return ret;
}
