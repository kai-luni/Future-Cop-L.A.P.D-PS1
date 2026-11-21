// Categories: Platform > CD-ROM > Control
#include "game/cdrom_globals.h"

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern int gPassCounter __asm__("D_800A3E6C");
extern int gPollDeadline __asm__("D_800A3E68");
extern char *gCdWaitFuncName __asm__("D_800A3E70");
extern char gCdReadyNameStr[] __asm__("D_80012518");
extern char gCdTimeoutFmtMsg[] __asm__("D_80012498");
extern char gCdErrorLogMsg[] __asm__("D_80012488");
extern int gLowByteEntryTable[] __asm__("D_8009C94C");
extern int gIndexEntryTable[] __asm__("D_8009C8CC");
extern u8 gLastResultByte __asm__("D_8009C8C5");
extern u8 gCdStateBytes[] __asm__("D_8009CB84");
extern void (*gCdReadyCallback)(int, void *) __asm__("D_8009C8AC");
extern void (*gCdSyncCallback)(int, void *) __asm__("D_8009C8A8");
extern volatile u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u8 gCdDataBuffer8[] __asm__("D_800A3E60");
extern u8 gCdReadyBuffer8[] __asm__("D_800A3E58");
extern u8 gStatusBuffer8[] __asm__("D_800A3E50");

extern int gVSyncWait(int) __asm__("func_80091270");
extern void gBiosPuts(char *) __asm__("func_8008FF08");
extern void gBiosPrintf(char *, char *, int, int) __asm__("func_8008E000");
extern void gResetCdHardware(void) __asm__("func_8008F6D8");
extern int gGetIrqLatchFlag(void) __asm__("func_800915D4");
extern int gCdDrainIrqEvent(void) __asm__("func_8008E7A0");
extern void dummy_5args(int, int, int, int, int);
s32 gCdReadyLoop(s32 mode, u8 *result) __asm__("func_8008EF7C");


/**
 * @brief Waits for a pending CD command result, with timeout diagnostics.
 *
 * Implements the CD_ready wait: polls the two result slots of the CD state
 * array D_8009CB84 (byte 2 first, then byte 1). A non-zero slot status is
 * cleared, its 8-byte result block (D_800A3E60 or D_800A3E58) is copied to
 * the caller buffer and the status returned. While waiting it drains CD
 * events through func_8008E7A0 and dispatches the registered callbacks
 * D_8009C8AC (event bit 4, slot 1) and D_8009C8A8 (event bit 2, slot 0).
 * Mode 0 blocks until a result arrives; any other mode polls once and
 * returns 0 when nothing is pending. The deadline is 0x3C0 frames from
 * func_80091270 and the iteration count is capped at 0x3C0000; on expiry it
 * prints 'CD timeout: ' via BIOS puts and the '%s:(%s) Sync=%s, Ready=%s'
 * report via BIOS printf using the status string tables, resets the CD
 * controller with func_8008F6D8 and returns -1.
 *
 * @param mode 0 blocks until completion, non-zero polls once.
 * @param result Optional 8-byte buffer receiving the command result block.
 * @return Slot status byte, 0 when nothing pending, -1 on timeout.
 *
 * @par Globals
 * D_8009CB84 - three-byte CD slot state; D_8009CB6C - CD index port pointer
 * saved/restored around event draining; D_8009C8AC/D_8009C8A8 - completion
 * callbacks; D_800A3E50/D_800A3E58/D_800A3E60 - result buffers;
 * D_8009C8CC/D_8009C94C with D_8009C8C5 - status string tables for the error
 * report; D_800A3E68/D_800A3E6C/D_800A3E70 - deadline, iteration counter and
 * the 'CD_ready' name pointer.
 *
 * @par Control flow
 * Outer poll loop with two timeout tests, an inner event-drain loop, then
 * the two slot checks in priority order; mode decides loop-or-return.
 *
 * @par Calls
 * func_80091270 - frame counter read for the deadline; func_800915D4 -
 * pending-event word D_8009CC8A; func_8008E7A0 - dequeues one CD event;
 * func_8008FF08 - BIOS puts B0(0x3F); func_8008E000 - BIOS printf A0(0x3F);
 * func_8008F6D8 - CD controller reset after timeout.
 *
 * @par Side effects
 * Clears slot status bytes, writes the result buffer, advances the iteration
 * counter, invokes callbacks and on timeout resets CD hardware.
 *
 * @par Callers
 * Confirmed: func_8008E174, which forwards mode/result queries here.
 *
 * @par Role and path
 * Polls or blocks for a pending CD-ROM result in gCdStateBytes[2]
 * (D_800A3E60) and gCdStateBytes[1] (D_800A3E58) with a 0x3C0-tick deadline
 * and 0x3C0000-iteration cap. Drains events via func_8008E7A0, dispatches
 * D_8009C8AC/D_8009C8A8 callbacks, copies the 8-byte result block, and resets
 * CD hardware via func_8008F6D8 on timeout. Driven by func_8008E174.
 */
s32 gCdReadyLoop(s32 mode, u8 *result)
{
    register int s0 __asm__("$16");
    register int s1 __asm__("$17");
    register u8 *state __asm__("$18");
    register s32 saved_mode __asm__("$23") = mode;
    register u8 *saved_result __asm__("$19") = result;
    register u8 *state2 __asm__("$20");
    register int *table __asm__("$21");
    register u8 *state1 __asm__("$22");
    int *other;
    char *msg;
    int ret;
    int now;
    register int status __asm__("$6");

    __asm__("" : "+r"(saved_mode));
    __asm__("" : "+r"(saved_result));

    now = gVSyncWait(-1);
    __asm__("lui $30, %%hi(D_8009C8CC)\n\taddiu $30, $30, %%lo(D_8009C8CC)" : "=r"(other));
    table = gLowByteEntryTable;
    state = gCdStateBytes;
    state1 = state + 1;
    state2 = state + 2;
    gPollDeadline = now + 0x3c0;
    msg = gCdReadyNameStr;
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
            gBiosPuts(gCdErrorLogMsg);
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
                gBiosPrintf(gCdTimeoutFmtMsg, m, third, fourth);
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
        sv = state2[0];
        __asm__("" : "+r"(sv));
        status = sv & 0xFF;
    }
    if (status != 0) {
        register u8 *src __asm__("$4");
        register u8 *dst __asm__("$5");
        state[2] = 0;
        __asm__ volatile("" : "=r"(src), "=r"(dst));
        src = gCdDataBuffer8;
        __asm__("" : "+r"(src));
        if (saved_result != 0) {
            int i;
            dst = saved_result;
            __asm__("" : "+r"(dst), "+r"(src));
            i = 7;
            do {
                *dst++ = *src++;
            } while (--i != -1);
        }
        return status;
    }

    {
        register int sv __asm__("$2");
        sv = state2[-1];
        __asm__("" : "+r"(sv));
        status = sv & 0xFF;
    }
    if (status != 0) {
        register u8 *src __asm__("$4");
        register u8 *dst __asm__("$5");
        state[1] = 0;
        __asm__ volatile("" : "=r"(src), "=r"(dst));
        dst = saved_result;
        src = gCdReadyBuffer8;
        __asm__("" : "+r"(dst), "+r"(src));
        if (dst != 0) {
            int i = 7;
            do {
                *dst++ = *src++;
            } while (--i != -1);
        }
        return status;
    }

    __asm__("" : "+r"(status));
    if (saved_mode == 0) goto loop;

    if (0) dummy_5args(0, 0, 0, 0, 0);

    return 0;
}
