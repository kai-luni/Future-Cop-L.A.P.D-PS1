// Categories: Graphics > Text > Debug Numbers
#include "game/types.h"

struct UnkB4E4 {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x18];
};

extern struct UnkB4E4 D_8009B4E4[];
extern s8 D_8009B72C[];
extern u8 D_8009F4E0;
extern u8 D_80011E60[];

extern void func_80082DDC(s32 arg0, s32 arg1);

/**
 * @brief Debug dump of five profile values (twin of func_80079FE0 with the
 *        D_8009B72C index table).
 *
 * @param arg0 Only action 3 runs the dump.
 * @return Nothing.
 *
 * @par Confirmed
 * Same loop as func_80079FE0 but over five entries of D_8009B72C; non-zero
 * values go to func_80082DDC(val, 2), D_8009F4E0 aborts, the fallback box
 * is func_80082DDC(D_80011E60, 0).
 *
 * @par Likely role
 * The five-slot variant of the profile inspection page (the loadout table
 * uses the same D_8009B72C indices).
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; likely a debug-only page (dispatch unconfirmed).
 */
void func_8007A0A4(s32 arg0)
{
    u32 i;
    s32 val;

    if (arg0 == 3) {
        i = 0;
        do {
            val = D_8009B4E4[D_8009B72C[i]].unk08;
            if (val != 0) {
                func_80082DDC(val, 2);
                if (D_8009F4E0 != 0) {
                    return;
                }
            }
            i++;
        } while (i < 5);
        if (D_8009F4E0 == 0) {
            func_80082DDC((s32)D_80011E60, 0);
        }
    }
}
