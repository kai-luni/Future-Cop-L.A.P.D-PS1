// Categories: Gameplay > Entity Script | Game Engine > Command Queue | Graphics > Animation
#include "game/types.h"

struct Command {
    s8 op;
    s8 arg;
};

struct Blah {
    u8 unk00[0x0A];
    struct Command cur;
    struct Command next;
};

/**
 * @brief Removes one entry from a 4-slot command queue at +0x0C by shifting later slots down.
 *
 * Consumes the executed 2-byte {op, arg} command at index within the 4-entry array starting at script + 0x0C, compacts any following entries toward index 0, and clears the trailing slot opcode at script[0x12].
 *
 * @param script Host block containing the four 2-byte command pairs at +0x0C..+0x13.
 * @param index 0-based slot index (0..3) to remove.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Starts at i = index + 1; if i < 4, loops copying each 2-byte Command pair from slot i down to slot i - 1 until i reaches 4, then unconditionally stores 0 to script[0x12] (slot 3 opcode).
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Shifts command bytes within script + 0x0C..+0x13 and clears script[0x12].
 *
 * @par Callers
 * Confirmed: func_8002B4B4 (0x8002B558), func_8002CB20 (0x8002CC00), func_8002E528 (0x8002E600), func_8002F280 (0x8002F2C8), func_80031004 (0x800310F0), func_800382B4 (0x80038534), func_80039BD0 (0x80039C24), func_8003A09C (0x8003A130), func_80041728 (0x8004183C), func_800425BC (0x8004266C), func_800449F8 (0x80044AB4), func_8004CECC (0x8004D368), func_8004F188 (0x8004F2F8), func_800585B8 (0x8005869C), func_8005FFB8 (0x800600B0).
 */
void func_8002A5B4(u8 *script, s32 index)
{
    register u8 *base __asm__("$8");
    register struct Blah *p __asm__("$6");
    s32 i;
    register s32 off __asm__("$2");

    base = script;
    i = index + 1;
    if (i < 4) {
        off = i * 2;
        p = (struct Blah *)(off + (u32)base);
        do {
            p->cur = p->next;
            i++;
            p = (struct Blah *)((u8 *)p + 2);
        } while (i < 4);
    }
    base[0x12] = 0;
}
