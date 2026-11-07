#include "game/types.h"

struct Stats { u8 pad[2]; u16 a; u8 p4[2]; u16 b; u8 p8[2]; u16 c; u8 pc[2]; u16 d; };
struct PlayerStats { u8 pad[0x2a]; u16 a; u8 p2c[2]; u16 b; u8 p30[2]; u16 c; u8 p34[2]; u16 d; };
struct Player { u8 pad[0x18]; u8 id; u8 p19[0x134-0x19]; struct PlayerStats *stats; };
struct Other { u8 pad[0x18]; u8 id; u8 p19[0xac-0x19]; struct Stats *stats; };
struct Info { u8 pad[0x1c]; u8 id, type; };
struct Sub { u8 pad[0x12]; s16 count; u8 p14[2]; u16 total; s32 arg1, arg2; u8 def[8]; struct Info *info; };
struct Entity { u8 pad[4]; struct Sub *sub; };
struct Mode { u8 pad[0x5c]; s32 active; };
extern struct Mode *D_8009FA94;
extern struct Player *D_8009F9EC;
extern struct Other *D_8009FA58, *D_8009FA5C;
extern s32 func_80069D68(s32, s32, s32, s32);
extern s32 func_8004D4F8(void *);
void func_8006125C(struct Entity *entity)
{
    struct Sub *s = entity->sub;
    struct Info *info;
    struct Stats *stats;
    struct Other *other;
    if (func_80069D68(s->arg1, 0, -1, 0)) {
        if (s->count > 0) s->count--;
        s->total++;
        if (D_8009FA94->active == 1) {
            info = s->info;
            if (D_8009F9EC && D_8009F9EC->id == info->id) {
                stats = 0;
            } else if ((other = D_8009FA58) && info->id == other->id) {
                stats = other->stats;
            } else if ((other = D_8009FA5C) && info->id == other->id) {
                stats = other->stats;
            } else goto done;
            switch (info->type) {
            case 1:
                if (stats) stats->a++; else D_8009F9EC->stats->a++;
                break;
            case 2:
                if (stats) stats->b++; else D_8009F9EC->stats->b++;
                break;
            case 3:
                if (stats) stats->c++; else D_8009F9EC->stats->c++;
                break;
            case 4:
                if (stats) stats->d++; else D_8009F9EC->stats->d++;
                break;
            }
        }
    done:
        func_8004D4F8(s->def);
        func_80069D68(s->arg2, 0, -1, 0);
    }
}
