// Categories: Platform > CD-ROM
#include "game/types.h"

extern s32 gCommandParamTable[] __asm__("D_8009C820");
extern s32 gCdSyncCallback __asm__("D_8009C8A8");
extern u8 gCdStatusByte __asm__("D_8009C8B4");
extern s32 gIssueCdCommand(s32, void *, void *, s32) __asm__("func_8008F244");
extern s32 gCdSyncLoop(s32, void *) __asm__("func_8008ECFC");
s32 gCdControlB(s32 command, void *param, void *result) __asm__("func_8008E42C");


/**
 * @brief Issues a CD command with a parameter block and an optional retry loop.
 *
 * The command byte selects the entry of D_8009C820. Up to three attempts are
 * made: each pass clears the callback word D_8009C8A8, and if the driver status
 * has bit 0x10 set the recovery call func_8008F244(1, 0, 0, 0) runs first. When
 * the entry exists and a parameter block was supplied, func_8008F244(2, param,
 * result, 0) prepares the transfer; the command itself follows as
 * func_8008F244(cmd, param, result, 0). After success the synchroniser
 * func_8008ECFC checks the reply: 2 means the command completed, so the function
 * returns 1, anything else 0.
 * @param command Command byte (masked with 0xFF) selecting the driver entry.
 * @param param Optional parameter block, may be null.
 * @param result Buffer that receives the reply.
 * @return 1 when the command completed (reply code 2), 0 on failure.
 * @par Globals
 * D_8009C820 - command/parameter table; D_8009C8A8 - callback word (saved and
 * restored around the inline recovery step); D_8009C8B4 - driver status.
 * @par Control flow
 * Three-pass do-while with two early successes.
 * @par Calls
 * func_8008F244 - command issue. func_8008ECFC - completion synchroniser.
 * @par Side effects
 * Transfers command parameters and may advance the driver state machine.
 * @par Callers
 * Confirmed: gCdStreamService (device start/stop).
 *
 * @par Role and path
 * Dispatches a CD-ROM command via func_8008F244 with up to four attempts
 * (including optional CdlNop and CdlSetloc) and then blocks in
 * func_8008ECFC(0, result). Returns 1 when func_8008ECFC reports status 2, or
 * 0 on failure. Driven by gCdStreamService.
 */
s32 gCdControlB(s32 command, void *param, void *result)
{
    register s32 i __asm__("$16");
    register void *p __asm__("$17") = param;
    register void *r __asm__("$18") = result;
    register s32 index __asm__("$19");
    register s32 cmd __asm__("$20") = command;
    register s32 saved __asm__("$21");
    register s32 *entry __asm__("$22");
    register s32 end __asm__("$23");
    s32 one;
    register s32 *base __asm__("$3");
    register s32 off __asm__("$2");
    register s32 call_a0 __asm__("$4");
    register void *zero_a1 __asm__("$5");
    register void *zero_a2 __asm__("$6");
    register s32 zero_a3 __asm__("$7");
    s32 ret;

    __asm__("" : "+r"(cmd));
    i = 3;
    one = 1;
    index = cmd & 0xff;
    __asm__("" : "+r"(index));
    base = gCommandParamTable;
    __asm__("" : "+r"(base));
    saved = gCdSyncCallback;
    __asm__("" : "+r"(saved));
    off = index << 2;
    __asm__("" : "+r"(off));
    entry = (s32 *)((unsigned int)off + (unsigned int)base);
    end = -1;
    __asm__("" : "+r"(end));
    do {
        __asm__ volatile(
            ".set\tnoreorder\n\t"
            ".set\tnoat\n\t"
            "lui\t$at,%%hi(D_8009C8A8)\n\t"
            "beq\t%0,%1,1f\n\t"
            "sw\t$zero,%%lo(D_8009C8A8)($at)\n\t"
            "lui\t$v0,%%hi(D_8009C8B4)\n\t"
            "lbu\t$v0,%%lo(D_8009C8B4)($v0)\n\t"
            "nop\n\t"
            "andi\t$v0,$v0,0x10\n\t"
            "beqz\t$v0,1f\n\t"
            "addiu\t$a0,$zero,1\n\t"
            "addu\t$a1,$zero,$zero\n\t"
            "addu\t$a2,$zero,$zero\n\t"
            "jal\tfunc_8008F244\n\t"
            "addu\t$a3,$zero,$zero\n"
            "1:\n\t"
            ".set\tat\n\t"
            ".set\treorder"
            :
            : "r"(index), "r"(one)
            : "$1", "$2", "$4", "$5", "$6", "$7", "$31", "memory"
        );
        if (p == 0 || *entry == 0 ||
            gIssueCdCommand(2, p, r, 0) == 0) {
            gCdSyncCallback = saved;
            if (gIssueCdCommand(cmd & 0xff, p, r, 0) == 0) {
                ret = 0;
                goto out;
            }
        }
        i--;
    } while (i != end);
    gCdSyncCallback = saved;
    ret = -1;
out:
    if (ret != 0) {
        return 0;
    }
    return gCdSyncLoop(0, r) == 2;
}
