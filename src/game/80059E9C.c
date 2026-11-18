// Categories: Gameplay > Player > Stats | Game Engine > Front-End Configuration
#include "game/types.h"
struct Player { u16 values[0x14]; s32 a; s32 b; u8 pad[0x91]; s8 index; };
struct State { u8 pad[0x134]; struct Defaults *defaults; };
struct Defaults { u8 pad[0x28]; u16 values[0]; };
struct Global { u8 pad[0xAC]; struct Player *player; };
extern s32 *gGameSession[] __asm__("D_8009FA94");
extern struct State *gSinglePlayerEntity[] __asm__("D_8009F9EC");
extern struct Global *gPlayerEntities[] __asm__("D_8009FA58");
/**
 * @brief Resolves a player stat id to its value, with per-player and
 *        default fallbacks and three special front-end words.
 *
 * @param player Player block (values[0x14], two s32 sums, index byte), or
 *        0 for the default context.
 * @param id Stat id.
 * @return The stat value.
 *
 * @par Confirmed
 * id < 15: player->values[id], or the defaults table behind
 * D_8009F9EC[0]->defaults when no player. Ids 12/13/14 instead read the
 * front-end block D_8009FA94[0] at word offsets 0x21/0x19/0x1F plus the
 * player index (without player: 0x22/0x1A/0x20). Ids 15..18 read the s32
 * at player + id*4 - 28 (defaults: +id*4 + 12); 17/18 return 0 without a
 * player. id >= 19 returns player->a + player->b, substituting the active
 * player from D_8009FA58[0] when null.
 *
 * @par Likely role
 * The stat source of the results-screen renderer func_80027F80: the low
 * ids are per-player counters, 12..14 the mission totals kept in the
 * front-end block, 19+ a combined score.
 *
 * @par Unknown
 * The exact stat vocabulary behind each id.
 *
 * @par Callers
 * func_80027F80.
 */
s32 func_80059E9C(struct Player *player, s32 id)
{
    s32 index;
    s32 *base;
    s32 *ptr;
    if (id < 15) {
        switch (id) {
        case 12:
            if (player) { index = player->index; base = gGameSession[0]; index += 0x21; goto indexed; }
            ptr = gGameSession[0] + 0x22; goto fetch;
        case 13:
            if (player) { index = player->index; base = gGameSession[0]; index += 0x19; goto indexed; }
            ptr = gGameSession[0] + 0x1A; goto fetch;
        case 14:
            if (player) {
                index = player->index; base = gGameSession[0]; index += 0x1F;
indexed:
                ptr = base + index;
                goto fetch;
            }
            ptr = gGameSession[0] + 0x20;
fetch:
            return *ptr;
        default:
            if (player) return player->values[id];
            return gSinglePlayerEntity[0]->defaults->values[id];
        }
    } else if (id < 19) {
        if (id >= 17) {
            if (player) goto player_value;
            return 0;
        }
        if (player) {
player_value:
          {
            register s32 offset __asm__("$2") = id * 4;
            __asm__ volatile("" : "=r"(offset) : "0"(offset));
            return *(s32 *)((char *)player + offset - 28);
          }
        }
        {
            register struct Defaults *base2 __asm__("$2") = gSinglePlayerEntity[0]->defaults;
            s32 offset;
            __asm__ volatile("" : "=r"(base2) : "0"(base2));
            offset = id * 4;
            return *(s32 *)((char *)base2 + offset + 12);
        }
    } else {
        if (!player) player = gPlayerEntities[0]->player;
        return player->a + player->b;
    }
}
