// Categories: Gameplay > Weapon Loadout | Game Engine > Difficulty
#include "game/types.h"

struct Data {
    u8 pad0[4];
    u8 kind;
    u8 flags;
    s16 value1;
    u8 pad8[2];
    u8 value2;
    u8 padB[0x1b];
    s16 value3;
};
struct State {
    u8 pad0[0x68];
    s32 field68;
    s32 field6c;
    s32 field70;
    u8 pad74[4];
    s32 field78[3];
    s16 field84[3];
    s16 field8a[3];
    u8 field90[3];
    u8 field93[3];
    u8 pad96[2];
    s32 field98;
    u8 pad9c[9];
    u8 fielda5;
    u8 pada6[0x1b];
    s8 fieldc1;
};
struct Owner { u8 pad[0xac]; struct State *state; };
struct Global { u8 pad[8]; s32 field8; u8 padc[0x8c]; s32 field98; };
extern struct Global *gGameSession __asm__("D_8009FA94");
extern struct Data *func_8006AAEC(u8);
extern void func_8005D3FC(struct Owner *, s32);

/**
 * @brief (Re)initialises the three weapon slots for a difficulty level.
 *
 * For each of the three slot indices in 0x90 the record is fetched with
 * func_8006AAEC and its two damage/range values are scaled by the active
 * difficulty field of D_8009FA94 (0x8 for the normal path, 0x98 when 0xC1 is
 * set): mode 1 adds a quarter, mode 2 adds a half and mode 3 sets both to 9999.
 * The pair is stored in 0x84/0x8A, the record's third value in 0x93 and the
 * per-slot counter is reset. A record with flag 4 clears 0xA5, and kind 13 runs
 * the per-slot setup func_8005D3FC. Finally the counters at 0x98, 0x6C, 0x70
 * are cleared and the active slot is set to -1.
 *
 * @param owner Object whose state block (0xAC) holds the three slots.
 *
 * @par Globals
 * D_8009FA94 - global difficulty block.
 *
 * @par Control flow
 * Three-iteration loop with a four-way switch on the difficulty mode.
 *
 * @par Calls
 * func_8006AAEC - weapon record lookup. func_8005D3FC - per-slot setup.
 *
 * @par Side effects
 * Rewrites the three weapon slots and clears the selection.
 *
 * @par Callers
 * Confirmed: func_800259D8.
 */
void func_8006AB38(struct Owner *owner)
{
    struct State *state;
    struct Data *data;
    s32 i, mode;
    register s32 a __asm__("$5");
    register s32 b __asm__("$4");
    state = owner->state;
    for (i = 0; i < 3; i++) {
        data = func_8006AAEC(state->field90[i]);
        a = data->value1;
        b = data->value3;
        if (owner->state->fieldc1 == 0)
            mode = gGameSession->field8;
        else
            mode = gGameSession->field98;
        switch (mode) {
        case 0:
            break;
        case 1:
            a += a >> 2;
            b += b >> 2;
            break;
        case 2:
            a += a >> 1;
            b += b >> 1;
            break;
        case 3:
            b = a = 9999;
            break;
        }
        state->field84[i] = a;
        state->field8a[i] = b;
        state->field93[i] = data->value2;
        state->field78[i] = 0;
        if (data->flags & 4)
            state->fielda5 = 0;
        if (data->kind == 13)
            func_8005D3FC(owner, i);
    }
    state->field98 = 0;
    state->field6c = 0;
    state->field70 = 0;
    state->field68 = -1;
}
