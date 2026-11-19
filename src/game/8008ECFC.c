// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/cdrom_globals.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern int gPassCounter __asm__("D_800A3E6C");
extern int gPollDeadline __asm__("D_800A3E68");
extern char *gCdWaitFuncName __asm__("D_800A3E70");
extern char gCdSyncNameStr[] __asm__("D_80012510");
extern char gCdTimeoutFmtMsg[] __asm__("D_80012498");
extern char gCdErrorLogMsg[] __asm__("D_80012488");
extern int gLowByteEntryTable[] __asm__("D_8009C94C");
extern int gIndexEntryTable[] __asm__("D_8009C8CC");
extern u8 gLastResultByte __asm__("D_8009C8C5");
extern u8 gCdStateBytes[] __asm__("D_8009CB84");
extern void (*gCdReadyCallback)(int, void *) __asm__("D_8009C8AC");
extern void (*gCdSyncCallback)(int, void *) __asm__("D_8009C8A8");
extern volatile u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u8 gCdReadyBuffer8[] __asm__("D_800A3E58");
extern u8 gStatusBuffer8[] __asm__("D_800A3E50");

extern int gVSyncWait(int) __asm__("func_80091270");
extern void func_8008FF08(char *);
extern void func_8008E000(char *, char *, int, int);
extern void gResetCdHardware(void) __asm__("func_8008F6D8");
extern int gGetIrqLatchFlag(void) __asm__("func_800915D4");
extern int gCdDrainIrqEvent(void) __asm__("func_8008E7A0");
extern void dummy_5args(int, int, int, int, int);
s32 gCdSyncLoop(s32 mode, u8 *result) __asm__("func_8008ECFC");


/**
 * @brief Polls or blocks for completion of the active CD-ROM command (CD_sync).
 *
 * Arms a 0x3C0-tick timeout from func_80091270(-1) in D_800A3E68 and resets D_800A3E6C. Each pass checks deadline and 0x3C0000 iteration cap (printing error diagnostics and calling func_8008F6D8 on timeout), drains CD events through func_8008E7A0 when func_800915D4 is non-zero, and returns status 2 or 5 (copying D_800A3E50 into result).
 *
 * @param mode 0 blocks until completion or timeout; non-zero polls once and returns 0 if still pending.
 * @param result Optional 8-byte buffer receiving D_800A3E50 on status 2 or 5.
 * @return Status byte 2 (done) or 5 (error), 0 if pending with mode != 0, or -1 on timeout.
 *
 * @par Globals
 * D_8009CB84 - CD slot state; D_800A3E50/D_800A3E58 - result buffers; D_8009C8A8/D_8009C8AC - CD callbacks; D_800A3E68..D_800A3E70 - timeout state; D_8009C8CC/D_8009C94C/D_8009C8C5 - log tables; D_8009CB6C - CD index port.
 *
 * @par Control flow
 * Outer loop with timeout guard, inner event-drain loop gated by func_800915D4(), status 2/5 exit, and mode == 0 loop test.
 *
 * @par Calls
 * func_80091270 - reads frame clock; func_800915D4 - checks D_8009CC8A; func_8008E7A0 - dequeues CD event; func_8008FF08/func_8008E000 - logs timeout; func_8008F6D8 - resets CD on timeout.
 *
 * @par Side effects
 * Updates D_800A3E68..D_800A3E70, preserves/restores *D_8009CB6C, resets D_8009CB84[0] to 2, writes result, and resets CD on timeout.
 *
 * @par Callers
 * Confirmed: func_8008E154 (0x8008E15C), func_8008E42C (0x8008E530), func_8008F244 (0x8008F304), func_8008F8E8 (0x8008FA94) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Polls or blocks for completion of the active CD-ROM command with a
 * 0x3C0-tick deadline in gPollDeadline (D_800A3E68) and 0x3C0000-iteration
 * cap in gPassCounter (D_800A3E6C). Drains events via func_8008E7A0 when
 * func_800915D4 is non-zero, fires D_8009C8AC/D_8009C8A8 callbacks, copies
 * gStatusBuffer8 (D_800A3E50) on status 2 or 5, and calls func_8008F6D8 on
 * timeout. Driven by func_8008E154, func_8008E42C, func_8008F244, and
 * func_8008F8E8.
 */
s32 gCdSyncLoop(s32 mode, u8 *result)
{
    register int s0 __asm__("$16");
    register int s1 __asm__("$17");
    register u8 *state __asm__("$18");
    register s32 two __asm__("$19");
    register int *table __asm__("$20");
    register u8 *state1 __asm__("$21");
    register s32 saved_mode __asm__("$22") = mode;
    register u8 *saved_result __asm__("$23") = result;
    int *other;
    char *msg;
    int ret;
    int now;
    int status;

    __asm__("" : "+r"(saved_mode));
    __asm__("" : "+r"(saved_result));

    now = gVSyncWait(-1);
    __asm__("lui $30, %%hi(D_8009C8CC)\n\taddiu $30, $30, %%lo(D_8009C8CC)" : "=r"(other));
    table = gLowByteEntryTable;
    state = gCdStateBytes;
    state1 = state + 1;
    two = 2;
    gPollDeadline = now + 0x3c0;
    msg = gCdSyncNameStr;
    gPassCounter = 0;
    gCdWaitFuncName = msg;

loop:
    if (gPollDeadline < gVSyncWait(-1)) goto timeout;
    {
        register int cur __asm__("$2");
        register int next __asm__("$2");
        register int old __asm__("$3");
        cur = gPassCounter;
        __asm__("" : "+r"(cur));
        old = cur;
        next = cur + 1;
        __asm__("" : "+r"(old), "+r"(next));
        gPassCounter = next;
        if (0x3c0000 < old) {
        timeout:
            func_8008FF08(gCdErrorLogMsg);
            {
                register int first __asm__("$4") = state[0];
                register int second __asm__("$2") = state[1];
                register char *m __asm__("$5") = gCdWaitFuncName;
                register int fifth __asm__("$3");
                register int index __asm__("$2");
                register int fourth __asm__("$7");
                register int third __asm__("$6");

                __asm__("" : "+r"(first), "+r"(second), "+r"(m));
                second = (second << 2) + (int)table;
                __asm__("" : "+r"(second));
                first <<= 2;
                __asm__("" : "+r"(first));
                fifth = *(int *)second;
                index = gLastResultByte;
                __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                first = first + (int)table;
                __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                index = (index << 2) + (int)other;
                __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));

                __asm__("sw $3, 16($sp)");
                third = *(int *)index;
                fourth = *(int *)first;
                __asm__("" : "+r"(third), "+r"(fourth));
                func_8008E000(gCdTimeoutFmtMsg, m, third, fourth);
            }
            gResetCdHardware();
            ret = -1;
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        return -1;
    }

    if (gGetIrqLatchFlag() != 0) {
        {
            register int v __asm__("$2") = *gCdromIndexStatusPtr;
            __asm__("" : "+r"(v));
            s1 = v & 3;
        }
inner_loop:
        s0 = gCdDrainIrqEvent();
        if (s0 != 0) {
            if (s0 & 4) {
                if (gCdReadyCallback != 0) {
                    gCdReadyCallback(*state1, gCdReadyBuffer8);
                }
            }
            {
                register int m2 __asm__("$2");
                m2 = s0 & 2;
                if (m2) {
                    if (gCdSyncCallback != 0) {
                        register int a0 __asm__("$4");
                        a0 = *state;
                        __asm__("" : "+r"(a0));
                        gCdSyncCallback(a0, gStatusBuffer8);
                    }
                }
            }
            goto inner_loop;
        }
        *gCdromIndexStatusPtr = s1;
    }

    {
        register int sv __asm__("$2");
        sv = state[0];
        __asm__("" : "+r"(sv));
        status = sv & 0xFF;
    }
    if (status == two || status == 5) {
        register u8 *src __asm__("$4");
        register u8 *dst __asm__("$5");
        state[0] = two;
        dst = saved_result;
        src = gStatusBuffer8;
        __asm__("" : "+r"(dst), "+r"(src));
        if (dst != 0) {
            int i = 7;
            do {
                *dst++ = *src++;
            } while (--i != -1);
        }
        return status;
    }

    if (saved_mode == 0) goto loop;

    if (0) dummy_5args(0, 0, 0, 0, 0);

    return 0;
}
