// Categories: Platform > Memory Card > File Operations | Resources > File Access
#include "game/types.h"

#define LOAD_ADDRESS(p, sym) __asm__("lui\t%0,%%hi(" #sym ")\n\taddiu\t%0,%0,%%lo(" #sym ")" : "=r"(p))

typedef struct {
    volatile s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
} GlobalState;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
} Entry;

extern GlobalState func_80010000;
extern Entry gFileSlotTable[] __asm__("D_800A6D78");
extern s32 gTransferError __asm__("D_8009F244");
extern u8 gCdReadCommand __asm__("D_8009F248");
extern s32 gCdCallbackActive __asm__("D_8009F618");
extern u8 gCdCommandParam[] __asm__("D_8009F610");
extern u8 gCdTargetLoc[] __asm__("D_8009F614");
extern u8 gCdCurrentLoc[] __asm__("D_8009F61C");
extern u8 gEventBuffer[] __asm__("D_800A0798");
extern u8 gNoOpCdReadCompletionStub[] __asm__("D_8001C60C");
extern u8 gCdSyncCallbackCode[] __asm__("D_8001C614");
extern volatile u8 gCdStatusReg __asm__("D_1F801800");
extern volatile u8 gCdInterruptReg __asm__("D_1F801803");

extern s32 gWaitCdLoaderIdle(s32 mode) __asm__("func_8001C478");
extern void gCopyCdSectorChunk(void) __asm__("func_8001CC6C");
extern void gByteOffsetToCdlLoc(u8 *dst, s32 arg1) __asm__("func_8001C494");
extern s32 gCompareCdlLoc(u8 *a, u8 *b) __asm__("func_8001C3F8");
extern void gCopyWord(void *dst, void *src) __asm__("func_8001C27C");
extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
extern s32 gCdReady(s32 mode, u8 *result) __asm__("func_8008E174");
extern s32 gSetCdSyncCallback(s32 ptr) __asm__("func_8008E194");
extern s32 gCdControl(s32 mode, void *src, void *dst) __asm__("func_8008E1BC");
s32 gStartCdFileRead(s32 arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_8001CE54");


/**
 * @brief Performs one memory-card file operation (read, write or delete) for a
 *        loader slot.
 *
 * @param arg0 Loader slot index (0..3).
 * @param arg1, arg2 Operation-specific values (offset/size or a source pointer).
 * @param arg3 Completion callback, registered with the card driver.
 * @return The card driver's result; negative values are propagated unchanged.
 *
 * @par Confirmed flow
 * The slot entry is resolved from D_800A6D78 (24 bytes per slot) and its kind
 * (entry->unk10) selects the operation. The function then runs the same
 * protective bracket as the other card routines - func_8008C9C0 /
 * func_8008C9E0 around the critical section, D_1F801800/D_1F801803 for the
 * interface registers, func_8008E174 to poll, func_8008E1BC to start the
 * transfer, func_8008E194 to install the callback - and copies the result into
 * the loader's global state (0x80010000).
 *
 * @par Likely role
 * The single entry point behind "save", "load" and "delete" for the front-end's
 * file slots; the kind byte decides which of the three.
 *
 * @par Unknown
 * The exact kind values (the switch is spread over the body, not a table).
 *
 * @par Role and path
 * Starts an asynchronous read on gFileSlotTable[arg0] after waiting for
 * func_8001C478(0x11), configuring 0x80010000 and copying cached sector data
 * via func_8001CC6C if D_8009F244 matches. Otherwise encodes the sector
 * CdlLOC into gCdTargetLoc via func_8001C494, clears
 * gCdStatusReg/gCdInterruptReg, issues CD command 0xE and gCdReadCommand via
 * func_8008E1BC, and installs D_8001C614 via func_8008E194; called by
 * func_8001D90C.
 */
s32 gStartCdFileRead(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    Entry *entry;
    GlobalState *st;
    s32 kind;
    register s32 result __asm__("$2");
    register GlobalState *liveState __asm__("$17");
    s32 previous;
    register s32 index __asm__("$18");
    register s32 base __asm__("$2");
    register s32 offset __asm__("$3");
    register s32 callback __asm__("$19");
    register u32 stateHigh __asm__("$16");
    register GlobalState *oldState __asm__("$2");
    register s32 second __asm__("$21");
    register s32 third __asm__("$20");
    register s32 fifth __asm__("$5");
    register u8 *a __asm__("$4");
    register u8 *b __asm__("$5");

    index = arg0;
    second = arg1;
    third = arg2;
    __asm__("" : : "r"(second), "r"(third));
    __asm__("" : : : "$19");
    callback = arg3;
    base = (s32)gFileSlotTable;
    offset = index * 24;
    entry = (Entry *)(offset + base);
    kind = entry->unk10;
    if (kind != 1 && kind != 0x11) {
        return -7;
    }
    result = gWaitCdLoaderIdle(0x11);
    if (result < 0) {
        return result;
    }
    entry->unk00 = 2;
    st = &func_80010000;
    st->unk14 = callback;
    if (callback == 0) {
        st->unk14 = (s32)gNoOpCdReadCompletionStub;
    }
    st->unk04 = third;
    st->unk10 = index;
    st->unk20 = second;
    st->unk08 = 0;
    if ((entry->unk0C & 0xFFFFF800) == gTransferError) {
        gCopyCdSectorChunk();
    }
    if (st->unk04 <= 0) {
        goto success;
    }
    fifth = entry->unk0C;
    LOAD_ADDRESS(a, D_8009F614);
    gByteOffsetToCdlLoc(a, fifth);
    while (gCdReady(1, 0) != 0) {
    }
    a = (u8 *)0xE;
    b = gCdCommandParam;
    __asm__("lui\t$2,%%hi(D_800A0798)\n\taddiu\t%0,$2,%%lo(D_800A0798)" : "=r"(index) : "r"(a), "r"(b) : "$2");
    gCdStatusReg = 0;
    gCdInterruptReg = 0;
    __asm__ volatile("" : "+r"(index) : : "memory");
    result = gCdControl((s32)a, b, (void *)index);
    if (result == 0) {
        stateHigh = 0x80010000;
        __asm__("addiu\t%0,%1,0" : "=r"(oldState) : "r"(stateHigh));
        previous = oldState->unk1C;
        goto failure;
    }
    if (gCdCallbackActive == 0) {
        gDisableCpuInterrupts();
    }
    LOAD_ADDRESS(a, D_8009F61C);
    LOAD_ADDRESS(b, D_8009F614);
    if (gCompareCdlLoc(a, b) == 0) {
        func_80010000.unk00 = 2;
        if (gCdCallbackActive != 0) {
            goto success;
        }
        gEnableCpuInterrupts();
        goto success;
    }
    stateHigh = 0x80010000;
    ((GlobalState *)stateHigh)->unk00 = 1;
    __asm__ volatile(
        "lw\t$2,D_8009F618\n\t"
        "nop\n\t"
        ".set\tnoreorder\n\t"
        ".set\tnomacro\n\t"
        "bne\t$2,$0,1f\n\t"
        "addiu\t$17,$16,0\n\t"
        "jal\tfunc_8008C9E0\n\t"
        "nop\n\t"
        ".set\tmacro\n\t"
        ".set\treorder\n"
        "1:"
        : "=r"(liveState)
        : "r"(stateHigh)
        : "$2"
    );
    liveState->unk1C = gSetCdSyncCallback((s32)gCdSyncCallbackCode);
    LOAD_ADDRESS(a, D_8009F61C);
    LOAD_ADDRESS(b, D_8009F614);
    gCopyWord(a, b);
    LOAD_ADDRESS(b, D_8009F614);
    if (gCdControl(gCdReadCommand, b, (void *)index) != 0) {
        goto success;
    }
    previous = liveState->unk1C;
failure:
    gSetCdSyncCallback(previous);
    result = -4;
    __asm__ volatile("sw\t$0,0(%0)" : : "r"(stateHigh), "r"(result) : "memory");
    return result;
success:
    oldState = &func_80010000;
    __asm__("" : "+r"(oldState));
    return oldState->unk08;
}
