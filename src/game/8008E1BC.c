// Categories: Platform > CD-ROM
#include "game/types.h"

extern s32 D_8009C820[];
extern s32 D_8009C8A8;
extern u8 gCdStatusByte __asm__("D_8009C8B4");
extern s32 func_8008F244(s32, void *, void *, s32);

/**
 * @brief Dispatches a synchronous CD-ROM command with up to four attempts and automatic Nop/Setloc handling.
 *
 * Saves and suppresses the driver callback D_8009C8A8 during preparatory commands, issues CdlNop (1) or CdlSetloc (2) when required by D_8009C8B4 and D_8009C820, then restores D_8009C8A8 and issues the command via func_8008F244.
 *
 * @param command Command byte (low 8 bits used as index into D_8009C820 and passed to func_8008F244).
 * @param param Optional command parameter block (e.g. CdlLOC for Setloc); null skips Setloc.
 * @param result Output buffer passed through to func_8008F244.
 * @return 1 if the command succeeded (func_8008F244 returned 0), or 0 after 4 failed attempts.
 *
 * @par Globals
 * D_8009C820 - per-command flag table; non-zero selects pre-issuing Setloc (cmd 2). D_8009C8A8 - driver callback word, temporarily cleared across setup. D_8009C8B4 - driver status byte; bit 0x10 triggers pre-command 1 when command != 1.
 *
 * @par Control flow
 * Loops up to 4 times (i = 3 down to 0): clears D_8009C8A8, runs func_8008F244(1, 0, 0, 0) if index != 1 and D_8009C8B4 & 0x10, runs func_8008F244(2, param, result, 0) when param != 0 and D_8009C820[index] != 0, then restores D_8009C8A8 and executes func_8008F244(index, param, result, 0); returns 1 on 0, or restores D_8009C8A8 and returns 0 on exhaustion.
 *
 * @par Calls
 * func_8008F244 - low-level CD-ROM primitive used for command 1 (Nop/sync), command 2 (Setloc), and the target command.
 *
 * @par Side effects
 * Temporarily clears and restores D_8009C8A8 and issues hardware CD-ROM commands via func_8008F244.
 *
 * @par Callers
 * Confirmed: func_8001C680 (0x8001C6BC), func_8001C718 (0x8001C78C), func_8001C808 (0x8001CBCC), func_8001CE54 (0x8001CF74, 0x8001D044), func_8001D0A0 (0x8001D108), func_8001D130 (0x8001D1CC, 0x8001D1DC, 0x8001D218), func_800886E4 (0x8008871C, 0x80088750), func_80088770 (0x8008884C), func_80089018 (0x80089094), func_8009081C (0x80090848), and func_80090C18 (0x80090CE4, 0x80090CFC, 0x80090D44).
 */
s32 func_8008E1BC(s32 command, void *param, void *result)
{
    register s32 i __asm__("$16");
    register void *p __asm__("$17") = param;
    register void *r __asm__("$18") = result;
    register s32 index __asm__("$19");
    register s32 cmd __asm__("$20") = command;
    register s32 saved __asm__("$21");
    register s32 *entry __asm__("$22");
    register s32 ret __asm__("$23");
    s32 end;
    register s32 *base __asm__("$3");
    register s32 off __asm__("$2");
    register s32 one __asm__("$8");
    register s32 call_a0 __asm__("$4");
    register void *zero_a1 __asm__("$5");
    register void *zero_a2 __asm__("$6");
    register s32 zero_a3 __asm__("$7");

    __asm__("" : "+r"(cmd));
    i = 3;
    index = cmd & 0xff;
    __asm__("" : "+r"(index));
    base = D_8009C820;
    __asm__("" : "+r"(base));
    saved = D_8009C8A8;
    __asm__("" : "+r"(saved));
    off = index << 2;
    __asm__("" : "+r"(off));
    entry = (s32 *)((unsigned int)off + (unsigned int)base);
    ret = 0;
    end = -1;
    __asm__("" : "+r"(ret), "+r"(end));
    do {
        D_8009C8A8 = 0;
        one = 1;
        __asm__("" : "+r"(one));
        if (index != one && (gCdStatusByte & 0x10)) {
            call_a0 = 1;
            __asm__("" : "+r"(call_a0));
            zero_a1 = 0;
            __asm__("" : "+r"(zero_a1));
            zero_a2 = 0;
            __asm__("" : "+r"(zero_a2));
            func_8008F244(call_a0, zero_a1, zero_a2, 0);
        }
        if (p == 0 || *entry == 0 ||
            func_8008F244(2, p, r, 0) == 0) {
            D_8009C8A8 = saved;
            if (func_8008F244(cmd & 0xff, p, r, 0) == 0) {
                return ret + 1;
            }
        }
        i--;
    } while (i != end);
    D_8009C8A8 = saved;
    ret = -1;
    __asm__("" : "+r"(ret));
    __asm__ volatile(".set reorder");
    return ret + 1;
}
