// Categories: Game Engine > Front-End Configuration | Gameplay > Loadout
typedef signed char s8;
typedef unsigned char u8;

struct Entry {
    unsigned int unknown;
    u8 *values;
    u8 unused;
    u8 replacement;
    u8 limit;
    u8 pad[9];
};
extern struct Entry D_8009C0A8[3];
extern s8 *D_8009F468;

/**
 * @brief Clamps out-of-range loadout selections in D_8009C0A8 back to their replacement defaults for both players.
 *
 * Iterates players i = 0..1 and the three 0x14-byte loadout entries D_8009C0A8[0..2].
 * Whenever entry.values[i] > entry.limit, resets it to entry.replacement, optionally sparing
 * the currently highlighted row when mode == 0.
 *
 * @param mode 0 (draw pass) skips the currently selected row for player 0/1; non-zero (confirm pass) clamps all rows.
 *
 * @par Globals
 * D_8009C0A8 - 3-element Entry table (values ptr at +0x04, replacement at +0x09, limit at +0x0A);
 * D_8009F468 - active menu state; bytes [0] and [1] give player 0 (+1 offset) and player 1 (+5 offset) row cursors.
 * @par Control flow
 * Nested 2x3 loops over players and entries with an inner mode == 0 guard skipping the focused row.
 * @par Calls
 * None.
 * @par Side effects
 * Overwrites over-limit entries in D_8009C0A8[j].values[i] with p->replacement.
 * @par Callers
 * Confirmed: func_8007DD08 (loadout draw pass, mode=0), func_8007E3EC (loadout confirm pass, mode=1).
 */
void func_8007D2DC(int mode)
{
    int i, j;
    struct Entry *p;
    struct Entry *base;
    u8 *value;
    i = 0;
    base = D_8009C0A8;
    for (; i < 2; i++) {
        for (j = 0, p = base; j < 3; j++, p++) {
            __asm__("" : "+r"(p));
            value = p->values + i;
            if (*value > p->limit) {
                if (mode == 0) {
                    if (i == 0 && j == D_8009F468[0] - 1)
                        continue;
                    if (i == 1 && j == D_8009F468[1] - 5)
                        continue;
                }
                *value = p->replacement;
            }
        }
    }
}
