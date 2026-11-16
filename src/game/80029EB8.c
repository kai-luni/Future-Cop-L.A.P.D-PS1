// Categories: Resources > Asset Registry | Game Engine > Game State > Reset | Resources > Reference Counting
#include "game/types.h"

struct Unk800A0B70 {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    u8 unk0C[0x20];
    s32 unk2C;
    u8 unk30[0x14];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
};

extern struct Unk800A0B70 D_800A0B70;
extern volatile u8 D_8009F6A8;

extern void func_80029DD8(void);
extern void func_80029520(void);
extern void func_80019584(void);
extern void func_8001D3A8(s32 arg0);

/**
 * @brief Resets the asset registry and loader state during subsystem teardown.
 *
 * Captures and clears active loader slot D_800A0B70.unk00, tears down stream/handle state via func_80029DD8, flushes deferred reference-counted handle queues via func_80029520, zeroes D_800A0B70 fields, frees pending buffer unk2C, and waits for D_8009F6A8 before clearing slot old.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A0B70 (asset registry/loader state: unk00, unk04, unk08, unk2C, unk44, unk48, unk4C); D_8009F6A8 (volatile busy flag polled until zero).
 *
 * @par Control flow
 * Saves old = D_800A0B70.unk00, writes -1, calls func_80029DD8 and func_80029520, clears D_800A0B70 fields, frees D_800A0B70.unk2C via func_80019584 (in $a0) if non-null, and if old != -1 spin-waits on D_8009F6A8 == 0 then calls func_8001D3A8(old).
 *
 * @par Calls
 * func_80029DD8 (stream/handle teardown), func_80029520 (flush D_800A0C30 release queues), func_80019584 (frees D_800A0B70.unk2C via func_800181D4), func_8001D3A8 (clears slot entry in D_800A6D78).
 *
 * @par Side effects
 * Resets D_800A0B70 fields, frees pending buffer unk2C, flushes handle queues, and clears loader slot old.
 *
 * @par Callers
 * Confirmed: func_800249BC (0x800249CC, subsystem teardown) and func_80024D14 (0x80024D50, global reset).
 */
void func_80029EB8(void)
{
    s32 old;
    register s32 pending __asm__("$4");

    old = D_800A0B70.unk00;
    D_800A0B70.unk00 = -1;
    func_80029DD8();
    func_80029520();
    D_800A0B70.unk04 = 0;
    D_800A0B70.unk08 = 0;
    D_800A0B70.unk44 = 0;
    D_800A0B70.unk48 = 0;
    D_800A0B70.unk4C = 0;
    pending = D_800A0B70.unk2C;
    if (pending != 0) {
        func_80019584();
        D_800A0B70.unk2C = 0;
    }
    if (old != -1) {
        while (D_8009F6A8 != 0) {
        }
        func_8001D3A8(old);
    }
}
