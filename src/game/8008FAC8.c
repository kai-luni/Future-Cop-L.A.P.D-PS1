// Categories: Platform > Device Wait | Platform > Memory Card
#include "game/dma_globals.h"

typedef unsigned char u8;
extern int D_800A3E68, D_800A3E6C;
extern char *D_800A3E70;
extern char D_80012594[], D_80012498[];
extern char gCdErrorLogMsg[] __asm__("D_80012488");
extern int D_8009C8CC[], D_8009C94C[];
extern u8 gLastResultByte __asm__("D_8009C8C5");
extern u8 gCdStateBytes[] __asm__("D_8009CB84");
extern int *gDma3ChannelCtrlPtr __asm__("D_8009CBB0");
extern int func_80091270(int);
extern void func_8008FF08(char *);
extern void func_8008E000(char *, char *, int, int);
extern void func_8008F6D8(void);

/**
 * @brief Waits for a device reply word with a tick timeout and a retry ceiling.
 *
 * The deadline D_800A3E68 is the current tick plus 0x3C0 and the pass counter
 * D_800A3E6C starts at zero. Each pass first compares the deadline with the
 * current tick; a pass counter above 0x3C0000 or an expired deadline goes to
 * the error path, which prints D_80012488 through func_8008FF08, reports the
 * state bytes and the table entries through func_8008E000 and returns -1 after
 * func_8008F6D8. On a normal pass the function returns 0 unless bit 0x1000000
 * of the reply word *D_8009CBB0 is set; with mode 0 the loop then waits again
 * and finally returns 1.
 *
 * @param mode 0 keeps retrying, non-zero performs a single pass.
 * @return -1 on timeout, 0 when the reply bit is clear, 1 when it is set.
 *
 * @par Globals
 * D_800A3E68 - deadline; D_800A3E6C - pass counter; D_800A3E70 - message pointer;
 * D_8009CB84 - state bytes; D_8009C94C and D_8009C8CC - value tables;
 * D_8009C8C5 - table index; D_8009CBB0 - reply word pointer.
 *
 * @par Control flow
 * Do-while loop with a shared timeout label.
 *
 * @par Calls
 * func_80091270 - tick counter. func_8008FF08 - error print.
 * func_8008E000 - formatted diagnostic. func_8008F6D8 - error teardown.
 *
 * @par Side effects
 * Writes the deadline and pass counter, prints on timeout.
 *
 * @par Callers
 * Likely: func_8008E5FC (same block).
 */
int func_8008FAC8(int mode)
{
    int ret;
    int pad[2];
    register int *table __asm__("$16");
    register u8 *state __asm__("$17");
    register int saved_mode __asm__("$18") = mode;
    register int *other __asm__("$19");
    register int limit __asm__("$20");
    int now;
    register int count __asm__("$3");
    register int next __asm__("$2");
    now = func_80091270(-1);
    __asm__("" : "=m"(pad));
    limit = 0x3c0000;
    other = D_8009C8CC;
    state = gCdStateBytes;
    table = D_8009C94C;
    D_800A3E68 = now + 0x3c0;
    D_800A3E6C = 0;
    D_800A3E70 = D_80012594;
    do {
        if (D_800A3E68 < func_80091270(-1)) goto timeout;
        next = D_800A3E6C;
        __asm__("" : "+r"(next));
        count = next;
        next = next + 1;
        __asm__("" : "+r"(count), "+r"(next));
        count = limit < count;
        __asm__("" : "+r"(count));
        {
            register char *base __asm__("$1");
            __asm__("lui %0,%%hi(D_800A3E6C)" : "=r"(base));
            *(int *)(base + 0x3e6c) = next;
        }
        if (count) {
        timeout:
            func_8008FF08(gCdErrorLogMsg);
            {
                register int first __asm__("$4") = state[0];
                register int second __asm__("$2") = state[1];
                register char *msg __asm__("$5") = D_800A3E70;
                __asm__("" : "+r"(first), "+r"(second), "+r"(msg));
                {
                    register int fifth __asm__("$3");
                    register int index __asm__("$2");
                    second = (second << 2) + (int)table;
                    __asm__("" : "+r"(second));
                    first <<= 2;
                    __asm__("" : "+r"(first));
                    fifth = *(int *)second;
                    index = gLastResultByte;
                    __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                    {
                        register int fourth __asm__("$7");
                        first = first + (int)table;
                        __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                        index = (index << 2) + (int)other;
                        __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                        {
                            register int third __asm__("$6");
                            __asm__ volatile("sw %0,16($sp)" : : "r"(fifth) : "memory");
                            third = *(int *)index;
                            fourth = *(int *)first;
                            __asm__("" : "+r"(third), "+r"(fourth), "+r"(fifth));
                            func_8008E000(D_80012498, msg, third, fourth);
                        }
                    }
                }
            }
            func_8008F6D8();
            ret = -1;
        } else {
            __asm__("addu %0,$0,$0" : "=r"(ret));
        }
        if (ret != 0) return -1;
        if (!(*gDma3ChannelCtrlPtr & 0x1000000)) return 0;
    } while (saved_mode == 0);
    return 1;
}
