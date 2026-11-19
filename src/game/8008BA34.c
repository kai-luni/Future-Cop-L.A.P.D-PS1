// Categories: Platform > Device I/O
#include "game/sio_globals.h"

#include "game/types.h"

typedef struct UnkBA34_Item {
    u8 unk00[0x0C];
    u8 *unk0C;
    u8 unk10[0xF0 - 0x10];
} UnkBA34_Item;

typedef struct UnkBA34_Arg {
    u8 unk00[0x0C];
    u8 *unk0C;
    u8 unk10[0x36 - 0x10];
    u8 unk36;
    u8 unk37[0x3C - 0x37];
    u8 *unk3C;
    u8 unk40[0x44 - 0x40];
    u8 unk44;
} UnkBA34_Arg;

extern void (*gPadStepErrorHook)(s32) __asm__("D_8009C728");
extern  u8 (*gPadTxByteHook)(UnkBA34_Arg *, s32) __asm__("D_8009C730");
extern void (*gPadConfigHook)() __asm__("D_8009C734");
extern void (*gPadDrainPortHook)(u8 *) __asm__("D_8009C748");
extern void (*gPadCompleteHook)(u8 *) __asm__("D_8009C74C");
extern UnkBA34_Item *gPadBasePort __asm__("D_8009C75C");
extern s32 gPadSlotIndex __asm__("D_8009C768");
extern s32 gPadExtEnableFlag __asm__("D_8009C774");
extern s32 gPadPendingCounts[2] __asm__("D_8009C780");
extern s32 gPadMotorValue __asm__("D_8009C7A0");
extern volatile u8 *gSio0RxDataPtr __asm__("D_8009C7A4");

extern s32 gExchangeSio0Byte(UnkBA34_Arg *, u8) __asm__("func_8008A9A8");
extern s32 gTransferSio0Byte(UnkBA34_Arg *, u8) __asm__("func_8008AB80");
extern s32 gWaitSio0AckClear(void) __asm__("func_8008ADB4");
extern void gWaitSio0TxReady(void) __asm__("func_8008AE44");
extern void gStartTimer2Timeout(s32) __asm__("func_8008C890");
s32 gRunPadTransfer(UnkBA34_Arg *arg0) __asm__("func_8008BA34");


/**
 *  @brief Runs a callback-driven sequence of device operations.
 *
 *  Processes an optional preliminary sequence, advances indexed item states,
 *  and performs remaining operations before recording a device byte.
 *  @param arg0 Operation context containing item and output pointers.
 *  @return 0 on completion, a negative operation result, or -3 on check failure.
 *  @par Globals
 *  D_8009C728, D_8009C730, D_8009C734, D_8009C748, and D_8009C74C are callbacks;
 *  D_8009C75C, D_8009C768, D_8009C774, D_8009C780, and D_8009C7A0 hold sequence state;
 *  D_8009C7A4 supplies the final byte.
 *  @par Control flow
 *  Loops over preliminary and remaining operations, switches on item state,
 *  and returns early on operation or check failure.
 *  @par Calls
 *  func_8008AB80 and func_8008A9A8 perform operations; func_8008C890 delays;
 *  func_8008ADB4 checks progress; func_8008AE44 finalizes the sequence.
 *  @par Side effects
 *  Invokes callbacks, changes global sequence state, and appends an output byte.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Runs a multi-phase pad transfer by calling D_8009C734, optionally
 * exchanging preliminary bytes via func_8008AB80 when D_8009C774 is nonzero
 * and the high nibble at *arg0->unk3C is 8, and then stepping D_8009C780[idx]
 * through states 4, 3, 1, and 0 while transferring bytes with func_8008A9A8
 * until gPadMotorValue is exhausted. Each byte step delays with
 * func_8008C890(0x3C) and checks func_8008ADB4(), returning -3 on check
 * failure or any negative transfer error. At completion it calls
 * func_8008AE44(), appends *gSio0RxDataPtr to arg0->unk3C, and calls
 * D_8009C728(0).
 */
s32 gRunPadTransfer(UnkBA34_Arg *arg0)
{
    s32 i;
    s32 off;
    s32 s4;
    s32 idx;
    u8 *ptr;
    s32 ret;

    gPadConfigHook();

    s4 = 0;
    if (gPadExtEnableFlag != 0 && ((*arg0->unk3C >> 4) == 8)) {
        s4 = (arg0->unk36 == 0);
    }

    if (s4 != 0) {
        for (i = -1, off = -0xF0; i < 4; i++, off += 0xF0) {
            if (--gPadMotorValue <= 0) {
                break;
            }
            if (i >= 0) {
                gPadConfigHook(arg0->unk0C + off);
            }
            ret = gTransferSio0Byte(arg0, gPadTxByteHook(arg0, 1));
            if (ret < 0) {
                return ret;
            }
            gStartTimer2Timeout(0x3C);
            if (gWaitSio0AckClear() == 0) {
                return -3;
            }
        }
    }

    idx = (gPadSlotIndex == 0);
    while (gPadMotorValue >= 2) {
        if (gPadPendingCounts[idx] < 0) {
            break;
        }
        if (gPadPendingCounts[idx] > 0) {
            ptr = gPadBasePort[idx].unk0C + gPadPendingCounts[idx] * 0xF0 - 0xF0;
            gPadDrainPortHook(ptr);
        }
        switch (gPadPendingCounts[idx]) {
        case 4:
            gPadPendingCounts[idx] = 3;
            break;
        case 3:
            gPadDrainPortHook(ptr - 0xF0);
            gPadPendingCounts[idx] = 1;
            break;
        case 0:
        case 1:
            ptr = (u8 *)&gPadBasePort[idx];
            gPadDrainPortHook(ptr);
            gPadCompleteHook(ptr);
            gPadPendingCounts[idx] = -1;
            break;
        }
        ret = gExchangeSio0Byte(arg0, gPadTxByteHook(arg0, s4));
        if (ret < 0) {
            return ret;
        }
        gStartTimer2Timeout(0x3C);
        if (gWaitSio0AckClear() == 0) {
            return -3;
        }
        --gPadMotorValue;
    }

    while (--gPadMotorValue > 0) {
        ret = gExchangeSio0Byte(arg0, gPadTxByteHook(arg0, s4));
        if (ret < 0) {
            return ret;
        }
        gStartTimer2Timeout(0x3C);
        if (gWaitSio0AckClear() == 0) {
            return -3;
        }
    }

    gWaitSio0TxReady();
    arg0->unk3C[arg0->unk44++] = *gSio0RxDataPtr;
    gPadStepErrorHook(0);
    return 0;
}
