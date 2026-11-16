// Categories: Game Engine > Event Queue | Platform > Streaming Audio | Gameplay > Entity Script
#include "game/types.h"

struct Unk41DC4;

struct Unk800A0BC0Entry {
    u16 unk00;
    u8 unk02;
    u8 pad03;
    struct Unk41DC4 *unk04;
};

struct Unk800A0BC0 {
    s16 unk00;
    s16 unk02;
    struct Unk800A0BC0Entry entries[8];
};

extern struct Unk800A0BC0 D_800A0BC0;
extern u32 D_8009FA00;
extern u8 D_8009F6B8;

extern void func_80029DD8(void);
extern void func_80041DC4(struct Unk41DC4 *object, s32 value);

/**
 * @brief Enqueues or priority-prepends an event/dialogue entry in the 8-slot D_800A0BC0 ring buffer.
 *
 * When arg2 == 0, rejects if D_800A0BC0 is non-empty or (D_8009F6B8 & 0x70) != 0; always rejects when D_8009FA00 >= 5. Otherwise calls func_80041DC4(arg1, arg0) and either appends at (unk02 + unk00) & 7 or, if arg2 != 0 and non-empty, tears down active audio via func_80029DD8 and prepends before unk02. Category Game Engine > Event Queue | Platform > Streaming Audio | Gameplay > Entity Script reflects the ring buffer, stream teardown, and script-VM caller.
 *
 * @param arg0 Event/dialogue ID written as u16 into slot unk00 and passed to func_80041DC4.
 * @param arg1 Entity pointer stored in slot unk04 (checked for kind == 0x22 in func_80041DC4).
 * @param arg2 Non-zero allows immediate prepend with stream teardown; 0 requires an idle empty queue.
 * @return 1 if queued, or 0 if blocked by queue occupancy, D_8009F6B8 & 0x70, or D_8009FA00 >= 5.
 *
 * @par Globals
 * D_800A0BC0 - 8-slot circular queue (unk00 count, unk02 base index, entries[]); D_8009F6B8/D_8009FA00 - stream flags and screen state.
 *
 * @par Control flow
 * Early guard checks returning 0, then branch between tail append and priority head prepend.
 *
 * @par Calls
 * func_80029DD8 - tears down active stream voices when preempting; func_80041DC4 - marks arg1 (kind == 0x22) and increments D_8009F9B0.
 *
 * @par Side effects
 * Writes a slot and updates unk00/unk02 in D_800A0BC0; updates entity/stream state via callees.
 *
 * @par Callers
 * Confirmed: func_80069D68 (0x8006A274 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_80029A60(s32 arg0, struct Unk41DC4 *arg1, u8 arg2)
{
    s32 index;

    if (arg2 == 0) {
        if (D_800A0BC0.unk00 > 0) {
            return 0;
        }
        if ((D_8009F6B8 & 0x70) != 0) {
            return 0;
        }
    }

    if (D_8009FA00 >= 5) {
        return 0;
    }

    if (arg2 == 0 || D_800A0BC0.unk00 <= 0) {
        func_80041DC4(arg1, arg0);

        index = (D_800A0BC0.unk02 + D_800A0BC0.unk00) & 7;
        D_800A0BC0.entries[index].unk00 = arg0;
        D_800A0BC0.entries[index].unk02 = 1;
        D_800A0BC0.entries[index].unk04 = arg1;
        D_800A0BC0.unk00++;
    } else {
        func_80029DD8();
        func_80041DC4(arg1, arg0);

        index = D_800A0BC0.unk02 - 1;
        if (index < 0) {
            index = 7;
        }

        D_800A0BC0.entries[index].unk00 = arg0;
        D_800A0BC0.entries[index].unk02 = 1;
        D_800A0BC0.entries[index].unk04 = arg1;
        D_800A0BC0.unk02 = index;

        if (D_800A0BC0.unk00 < 8) {
            D_800A0BC0.unk00++;
        }
    }

    return 1;
}
