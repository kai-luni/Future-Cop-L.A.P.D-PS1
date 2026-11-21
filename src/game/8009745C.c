// Categories: Platform > SPU Transfer
#include "game/dma_globals.h"

#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    u8 pad00[0x1A6];
    volatile u16 field1A6;
    u8 pad1A8[2];
    volatile u16 field1AA;
} SpuDebug;

extern SpuDebug *gSpuRegBasePtr __asm__("D_8009EEA4");
extern s32 gSpuAddrShift __asm__("D_8009EECC");
extern u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern s32 gSetupGuard __asm__("D_8009EEF4");
extern s32 gSpuDmaMainRamAddress __asm__("D_8009EEF8");
extern s32 D_8009EEFC;
extern s32 *gDma4MemAddrPtr __asm__("D_8009EEA8");
extern s32 *gDma4BlockCtrlPtr __asm__("D_8009EEAC");
extern s32 *gDma4ChannelCtrlPtr __asm__("D_8009EEB0");

extern void func_80097940(void);
extern void func_80097968(void);

/**
 *  @brief Configures or starts an SPU transfer according to an operation code.
 *
 *  Sets a shifted transfer address, selects a transfer mode, or programs
 *  transfer registers after polling the SPU for the expected state.
 *  @param arg0 Operation code: 2 sets address, 0 or 1 selects mode, 3 starts transfer.
 *  @param ... Address for operation 2, or transfer address and byte count for operation 3.
 *  @return 0 on completion or an unrecognized operation; -2 on polling timeout.
 *  @par Globals
 *  D_8009EEA4 accesses SPU registers; D_8009EECC and D_8009EEBC set and track the address.
 *  D_8009EEF4 tracks mode; D_8009EEF8 and D_8009EEFC store transfer parameters.
 *  D_8009EEA8, D_8009EEAC and D_8009EEB0 point to transfer registers.
 *  @par Control flow
 *  Dispatches by operation and polls up to 0xF01 iterations before timing out.
 *  @par Calls
 *  func_80097968 or func_80097940 prepares the transfer according to mode.
 *  @par Side effects
 *  Updates transfer globals and writes SPU and transfer registers.
 *  @par Callers
 *  Confirmed: func_800976DC, func_80097760.
 */
s32 gSpuTransfer(s32 arg0, ...) __asm__("func_8009745C");

s32 gSpuTransfer(s32 arg0, ...)
{
    s32 *vl;
    u16 target;
    u32 poll;
    u32 want;
    u32 i;
    register u32 rx __asm__("$4");
    register s32 rs __asm__("$2");
    register SpuDebug *rp __asm__("$3");
    register s32 *re __asm__("$5");
    register u32 rq __asm__("$3");
    register u32 rm __asm__("$2");
    register s32 rf __asm__("$3");
    u32 val;

    vl = (s32 *)((u32)&arg0 + 4);
    switch (arg0) {
    case 2:
        rx = (u32)*vl;
        rs = gSpuAddrShift;
        rp = gSpuRegBasePtr;
        poll = rx >> rs;
        gSpuTransferAddr = poll;
        rp->field1A6 = poll;
        break;
    case 1:
        gSetupGuard = 0;
        want = gSpuTransferAddr;
        i = 0;
        while (gSpuRegBasePtr->field1A6 != want) {
            if (++i >= 0xF01) {
                return -2;
            }
        }
        gSpuRegBasePtr->field1AA = (gSpuRegBasePtr->field1AA & 0xFFCF) | 0x20;
        break;
    case 0:
        gSetupGuard = 1;
        want = gSpuTransferAddr;
        i = 0;
        while (gSpuRegBasePtr->field1A6 != want) {
            if (++i >= 0xF01) {
                return -2;
            }
        }
        gSpuRegBasePtr->field1AA |= 0x30;
        break;
    case 3:
        target = (gSetupGuard == 1) ? 0x30 : 0x20;
        i = 0;
        while ((gSpuRegBasePtr->field1AA & 0x30) != target) {
            if (++i >= 0xF01) {
                return -2;
            }
        }
        if (gSetupGuard == 1) {
            func_80097968();
        } else {
            func_80097940();
        }
        vl = vl + 1;
        rx = (u32)vl[-1];
        gSpuDmaMainRamAddress = rx;
        rx = (u32)*vl;
        re = gDma4MemAddrPtr;
        rq = rx >> 6;
        rm = rx & 0x3F;
        D_8009EEFC = rq + (rm != 0);
        *gDma4MemAddrPtr = gSpuDmaMainRamAddress;
        *gDma4BlockCtrlPtr = (D_8009EEFC << 16) | 0x10;
        rf = gSetupGuard;
        val = (rf == 1) ? 0x1000200 : 0x1000201;
        *gDma4ChannelCtrlPtr = val;
        break;
    }
    return 0;
}
