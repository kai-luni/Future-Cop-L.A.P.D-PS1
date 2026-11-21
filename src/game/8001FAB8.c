// Categories: Platform > Device I/O
#include "game/types.h"

struct S {
    u8 p0[0x8F4];
    volatile s32 s8F4;
    volatile s32 s8F8;
    volatile s32 s8FC;
    volatile s32 s900;
    volatile s32 s904;
    volatile s32 s908;
    volatile s32 s90C;
    s32 s910;
    volatile u32 d[32];
    volatile s32 s994;
    volatile s32 s998;
    volatile s32 s99C;
    volatile s32 s9A0;
    volatile s32 s9A4;
    volatile s32 s9A8;
    u8 p9AC[4];
    u8 s9B0[0x1BB0 - 0x9B0];
    volatile s32 s1BB0;
};

extern struct S *gOperationState __asm__("D_8009AB08");

extern void gBiosCardInfo(s32) __asm__("func_8008D640");
extern void gBiosCardLoad(s32) __asm__("func_8008D650");
extern void gBiosWriteCardSector(s32, s32, volatile u32 *) __asm__("func_8008D670");
extern void gBiosCardReadSector(s32, s32, volatile u32 *) __asm__("func_8008D680");
extern void *gCopyMemory(volatile void *, void *, u32) __asm__("func_8001E318");
s32 gMcDispatchStep(void) __asm__("func_8001FAB8");


/**
 *  @brief Advances a global device-transfer state machine.
 *
 *  Selects a pending operation, prepares its 128-byte buffer, and dispatches
 *  a device request or completion action according to the stored state.
 *  @return Always 0.
 *  @par Globals
 *  D_8009AB08 points to the state, transfer buffer, and device identifier.
 *  @par Control flow
 *  Branches on request and operation states; one path loops over buffer words
 *  to calculate a checksum before dispatching a transfer.
 *  @par Calls
 *  func_8001E318 copies data; func_8008D670 and func_8008D680 dispatch
 *  transfers; func_8008D640 and func_8008D650 handle other device actions.
 *  @par Side effects
 *  Updates state fields and buffer contents and invokes device operations.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Advances the memory-card state machine in gOperationState (D_8009AB08):
 * clears s1BB0 and sets s9A8 to 1, then handles initial directory probe (s8F4
 * == 1) via func_8008D680, card probe (s908 != 0 && s904 == 0) via
 * func_8008D650, or queued commands s8F8 == 2/3/6 via func_8008D680 and
 * func_8008D670 (computing checksums when s90C is set), otherwise stopping
 * via func_8008D640.
 */
s32 gMcDispatchStep(void)
{
    register struct S *p asm("$3");
    register s32 a asm("$4");
    u32 i;
    s32 sum;
    volatile u32 *e;
    s32 x, y;
    struct S *q;

    p = gOperationState;
    p->s1BB0 = 0;
    p->s9A8 = 1;
    a = p->s8F4;
    if (a == 1) {
        p->s904 = a;
        if (p->s8F8 != 0) {
            p->s900 = -3;
            p->s8F8 = 0;
            p->s8F4 = 0;
            p->s904 = 0;
            gBiosCardInfo(p->s8FC);
            return 0;
        }
        e = p->d;
        p->s998 = 0;
        p->s99C = 0x10;
        p->s90C = 0;
        p->s994 = 0;
        p->s9A4 = 0;
        p->s9A0 = 3;
        p->s8F4 = 2;
        p->s8F8 = a;
        x = p->s8FC;
        y = p->s998;
        p->s910 = (s32)p->s9B0;
        gBiosCardReadSector(x, y, e);
        return 0;
    }
    sum = 0;
    if (p->s908 != 0 && p->s904 == 0) {
        p->s908 = 1;
        p->s8F4 = 1;
        gBiosCardLoad(p->s8FC);
        return 0;
    }
    if (gOperationState->s8F8 == 5) {
        gOperationState->s904 = 1;
        gOperationState->s8F8 = 6;
    }
    q = gOperationState;
    if (q->s8F8 == 2) {
        q->s9A0 = 3;
        q->s8F4 = 2;
        q->s8F8 = 0;
        q->s900 = -4;
        gBiosCardReadSector(q->s8FC, q->s998, q->d);
        return 0;
    }
    if (q->s8F8 == 3 || q->s8F8 == 6) {
        q->s9A0 = 3;
        q->s8F4 = q->s8F8;
        q->s8F8 = 0;
        if (q->s90C != 0) {
            gCopyMemory(&q->d[2], (void *)q->s910, 0x78);
            q = gOperationState;
            for (i = 2; i < 32; i++) {
                sum += (q->d[i] ^ q->d[32 - i]) + i;
            }
            gOperationState->d[0] = gOperationState->s998;
            gOperationState->d[1] = sum;
        } else {
            gCopyMemory(q->d, (void *)q->s910, 0x80);
        }
        q = gOperationState;
        q->s900 = -5;
        gBiosWriteCardSector(q->s8FC, q->s998, q->d);
        return 0;
    }
    q->s8F4 = 0;
    gBiosCardInfo(q->s8FC);
    return 0;
}
