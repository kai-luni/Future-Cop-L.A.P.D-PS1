// Categories: Platform > File Service
#include "game/types.h"

extern s32 gCommandParamTable[] __asm__("D_8009C820");
extern s32 D_8009C8A8;
extern u8 gCdStatusByte __asm__("D_8009C8B4");
extern s32 func_8008F244(s32, void *, void *, s32);

/**
 * @brief Retries a service request up to four times around a recovery step.
 *
 * Each pass first runs an inline assembly block: when the index equals 1 the
 * status word D_8009C8A8 is cleared, and if bit 0x10 of D_8009C8B4 is set a
 * recovery call func_8008F244(1, 0, 0, 0) is issued. Then, only if a buffer was
 * supplied and the indexed table entry is non-zero, func_8008F244(2, buffer, 0, 0)
 * is tried first. The actual request follows as func_8008F244(index, buffer, 0, 1);
 * a zero return means success and the function returns 1, otherwise the saved
 * status is restored and the pass repeats. After four failed passes it returns
 * 0 with the saved status restored.
 *
 * @param arg0 Request index, low 8 bits select the table entry.
 * @param arg1 Buffer handed to the service, may be null.
 * @return 1 on success, 0 when all four attempts failed.
 *
 * @par Globals
 * D_8009C820 - request table; D_8009C8A8 - status word (saved and restored);
 * D_8009C8B4 - status flags.
 *
 * @par Control flow
 * Do-while over four attempts with a success exit in the middle.
 *
 * @par Calls
 * func_8008F244 - the service entry point, called with three different modes.
 *
 * @par Side effects
 * Transiently clears D_8009C8A8 and always restores it.
 *
 * @par Callers
 * Confirmed: func_8001C680, func_8001C718. Likely: func_8008888C, func_800908D4,
 * func_80090B48, func_80090C18, func_80090E00.
 */
s32 func_8008E2F8(s32 arg0, void *arg1)
{
    register s32 i __asm__("$16");
    register void *a1 __asm__("$17") = arg1;
    register s32 index __asm__("$18");
    register s32 a0 __asm__("$19") = arg0;
    register s32 saved __asm__("$20");
    register s32 *entry __asm__("$21");
    register s32 result __asm__("$22");
    register s32 end __asm__("$23");
    register s32 one;
    register s32 *base __asm__("$3");
    register s32 off __asm__("$2");
    register s32 call_a0 __asm__("$4");
    register void *call_a1 __asm__("$5");
    register void *zero_a2 __asm__("$6");

    __asm__("" : "+r"(a0));
    i = 3;
    one = 1;
    __asm__("" : "=r"(one) : "0"(one));
    index = a0 & 0xff;
    __asm__("" : "+r"(index));
    base = gCommandParamTable;
    __asm__("" : "+r"(base));
    saved = D_8009C8A8;
    __asm__("" : "+r"(saved));
    off = index << 2;
    __asm__("" : "+r"(off));
    entry = (s32 *)((unsigned int)off + (unsigned int)base);
    result = 0;
    end = -1;
    __asm__("" : "+r"(result), "+r"(end));
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
        if (a1 != 0 && *entry != 0) {
            call_a0 = 2;
            __asm__("" : "+r"(call_a0));
            call_a1 = a1;
            __asm__("" : "+r"(call_a1));
            zero_a2 = 0;
            __asm__("" : "+r"(zero_a2));
            if (func_8008F244(call_a0, call_a1, zero_a2, 0) != 0) {
                goto next;
            }
        }
        D_8009C8A8 = saved;
        if (func_8008F244(a0 & 0xff, a1, 0, 1) == 0) {
            return result + 1;
        }
next:
        i--;
    } while (i != end);
    D_8009C8A8 = saved;
    result = -1;
    __asm__("" : "+r"(result));
    __asm__ volatile(".set reorder");
    return result + 1;
}
