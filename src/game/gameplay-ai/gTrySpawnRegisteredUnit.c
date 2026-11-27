// Categories: Gameplay > Statistics
#include "game/types.h"

struct Stats { u8 pad[2]; u16 a; u8 p4[2]; u16 b; u8 p8[2]; u16 c; u8 pc[2]; u16 d; };
struct PlayerStats { u8 pad[0x2a]; u16 a; u8 p2c[2]; u16 b; u8 p30[2]; u16 c; u8 p34[2]; u16 d; };
struct Player { u8 pad[0x18]; u8 id; u8 p19[0x134-0x19]; struct PlayerStats *stats; };
struct Other { u8 pad[0x18]; u8 id; u8 p19[0xac-0x19]; struct Stats *stats; };
struct Info { u8 pad[0x1c]; u8 id, type; };
struct Sub { u8 pad[0x12]; s16 count; u8 p14[2]; u16 total; s32 arg1, arg2; u8 def[8]; struct Info *info; };
struct Entity { u8 pad[4]; struct Sub *sub; };
struct Mode { u8 pad[0x5c]; s32 active; };
extern struct Mode *gGameSession __asm__("D_8009FA94");
extern struct Player *gSinglePlayerEntity __asm__("D_8009F9EC");
extern struct Other *gPlayerEntities __asm__("D_8009FA58");
extern struct Other *gPlayerEntity2 __asm__("D_8009FA5C");
extern s32 gInterpretEntityScript(s32, s32, s32, s32) __asm__("func_80069D68");
extern s32 gSpawnTargetableEntityId(void *) __asm__("func_8004D4F8");
void gTrySpawnRegisteredUnit(struct Entity *entity) __asm__("func_8006125C");


/**
 *  @brief Updates entity counters and matching participant statistics after a successful check.
 *
 *  On success, decrements a positive remaining count and increments the total.
 *  In active mode, the associated ID and type select a participant statistic to increment.
 *  @param entity Entity whose substructure supplies the check arguments and counters.
 *  @par Globals
 *  D_8009FA94 supplies the active mode; D_8009F9EC identifies the player.
 *  D_8009FA58 and D_8009FA5C identify other participants and their statistics.
 *  @par Control flow
 *  A failed initial check does nothing; otherwise IDs are matched in player-first order.
 *  Types 1 through 4 select counters; unmatched IDs skip the statistics update.
 *  @par Calls
 *  func_80069D68 performs the initial check and is called again with the second argument.
 *  func_8004D4F8 processes the substructure definition after a successful check.
 *  @par Side effects
 *  Changes entity counters and possibly a participant statistic; called functions may have effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gTrySpawnRegisteredUnit(struct Entity *entity)
{
    struct Sub *s = entity->sub;
    struct Info *info;
    struct Stats *stats;
    struct Other *other;
    if (gInterpretEntityScript(s->arg1, 0, -1, 0)) {
        if (s->count > 0) s->count--;
        s->total++;
        if (gGameSession->active == 1) {
            info = s->info;
            if (gSinglePlayerEntity && gSinglePlayerEntity->id == info->id) {
                stats = 0;
            } else if ((other = gPlayerEntities) && info->id == other->id) {
                stats = other->stats;
            } else if ((other = gPlayerEntity2) && info->id == other->id) {
                stats = other->stats;
            } else goto done;
            switch (info->type) {
            case 1:
                if (stats) stats->a++; else gSinglePlayerEntity->stats->a++;
                break;
            case 2:
                if (stats) stats->b++; else gSinglePlayerEntity->stats->b++;
                break;
            case 3:
                if (stats) stats->c++; else gSinglePlayerEntity->stats->c++;
                break;
            case 4:
                if (stats) stats->d++; else gSinglePlayerEntity->stats->d++;
                break;
            }
        }
    done:
        gSpawnTargetableEntityId(s->def);
        gInterpretEntityScript(s->arg2, 0, -1, 0);
    }
}
