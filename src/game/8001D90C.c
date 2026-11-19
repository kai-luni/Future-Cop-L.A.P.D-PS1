// Categories: Resources > File Access | Platform > Memory Card
typedef signed int s32;
typedef unsigned int u32;
struct Entry { volatile s32 state; s32 start, size, pos, flags, pad; };
extern struct Entry gFileSlotTable[] __asm__("D_800A6D78");
extern char func_80010010[];
extern s32 gSeekCdFileSlot(s32) __asm__("func_8001D0A0");
extern s32 gStartCdFileRead(s32, s32, s32, void (*)(s32,s32)) __asm__("func_8001CE54");
s32 gReadFileSlotAsync(s32 index, volatile s32 arg1, s32 arg2, void (*arg3)(s32,s32)) __asm__("func_8001D90C");


/**
 * @brief Executes a bounded read/write transfer on an open file slot in D_800A6D78.
 *
 * Validates slot index (0..3) and non-busy state, waits out states 2/3, marks state 3, checks pos-start against size (returning -14 on EOF), clamps count to remaining bytes (clearing byte 0x80010024 when flags == 0x11 reaches EOF), runs func_8001D0A0 for non-type-1 slots, calls func_8001CE54, advances pos, and invokes the completion callback.
 *
 * @param index Slot index 0..3 into D_800A6D78.
 * @param arg1 Transfer buffer or address passed to func_8001CE54.
 * @param arg2 Requested byte count, clamped to remaining slot size.
 * @param arg3 Optional completion callback invoked as arg3(transferred, status).
 * @return Transferred byte count on success, or negative status (-9, -14, or callee error).
 *
 * @par Globals
 * D_800A6D78 - 4-slot file table (state, start, size, pos, flags); byte 0x80010024 cleared on flags == 0x11 EOF.
 * @par Control flow
 * Busy-wait loop on states 2/3, EOF clamp, conditional pre-transfer setup when (flags & 15) != 1, transfer call, and error/completion branches.
 * @par Calls
 * func_8001D0A0 - prepares non-type-1 transfer slot; func_8001CE54 - performs card/file I/O operation; arg3 - completion callback.
 * @par Side effects
 * Updates D_800A6D78[index].state and .pos, clears 0x80010024 on flags == 0x11 EOF, and fires arg3.
 * @par Callers
 * Confirmed: func_8001DB60 (0x8001DBBC, 0x8001DC1C), func_8001E004 (0x8001E0B8) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Waits out busy states 2/3 on gFileSlotTable[index] (D_800A6D78), clamps
 * count to remaining bytes, seeks via func_8001D0A0 when (flags & 15) != 1,
 * and starts the transfer via func_8001CE54(index, arg1, count, callback).
 * Updates slot pos/state and invokes the completion callback when synchronous
 * or on error; called by func_8001DB60 and func_8001E004.
 */
s32 gReadFileSlotAsync(s32 index, volatile s32 arg1, s32 arg2, void (*arg3)(s32,s32))
{
    register s32 count __asm__("$19") = arg2;
    register void (*callback)(s32,s32) __asm__("$23") = arg3;
    register s32 state __asm__("$16");
    register s32 transferred __asm__("$18");
    register s32 flags __asm__("$21");
    s32 end = 0;
    register s32 type __asm__("$20");
    register s32 one __asm__("$22");
    s32 diff;
    register s32 finaltype __asm__("$3");
    register u32 test __asm__("$2");
    flags = gFileSlotTable[index].flags;
    do {
        state = -9;
        if ((u32)index < 4 && gFileSlotTable[index].state != 0)
            state = gFileSlotTable[index].state;
        if (state < 0) return state;
        if (state & 0x100) return -14;
        test = state - 2;
        __asm__("" : "+r"(test));
    } while (test < 2);
    gFileSlotTable[index].state = 3;
    __asm__("" ::: "memory");
    diff = gFileSlotTable[index].pos - gFileSlotTable[index].start;
    if (diff >= gFileSlotTable[index].size) {
        gFileSlotTable[index].state = 0x101;
        return -14;
    }
    if (diff + count >= gFileSlotTable[index].size) {
        count = gFileSlotTable[index].size - diff;
        end = 0x100;
        if (flags == 0x11) func_80010010[20] = 0;
    }
    state = 0;
    type = flags & 15;
    one = 1;
    transferred = state;
    if (type != one) {
        gFileSlotTable[index].state = 2;
        state = gSeekCdFileSlot(index);
    }
    if (state >= 0) {
        state = gStartCdFileRead(index, arg1, count, callback);
        if (state >= 0) {
            transferred = state;
            state = 0;
            if (type != one) gFileSlotTable[index].pos += transferred;
            __asm__("" : "+r"(state));
        }
    }
    __asm__("" : "+r"(flags));
    finaltype = flags & 15;
    if (state < 0) {
        gFileSlotTable[index].state = 1;
        if (callback) callback(transferred, state);
        return state;
    }
    if (finaltype != 1) {
        gFileSlotTable[index].state = end | 1;
        if (callback) callback(transferred, state);
    }
    return transferred;
}
