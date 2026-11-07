#include "game/types.h"
struct Player { u16 values[0x14]; s32 a; s32 b; u8 pad[0x91]; s8 index; };
struct State { u8 pad[0x134]; struct Defaults *defaults; };
struct Defaults { u8 pad[0x28]; u16 values[0]; };
struct Global { u8 pad[0xAC]; struct Player *player; };
extern s32 *D_8009FA94[];
extern struct State *D_8009F9EC[];
extern struct Global *D_8009FA58[];
s32 func_80059E9C(struct Player *player, s32 id)
{
    s32 index;
    s32 *base;
    s32 *ptr;
    if (id < 15) {
        switch (id) {
        case 12:
            if (player) { index = player->index; base = D_8009FA94[0]; index += 0x21; goto indexed; }
            ptr = D_8009FA94[0] + 0x22; goto fetch;
        case 13:
            if (player) { index = player->index; base = D_8009FA94[0]; index += 0x19; goto indexed; }
            ptr = D_8009FA94[0] + 0x1A; goto fetch;
        case 14:
            if (player) {
                index = player->index; base = D_8009FA94[0]; index += 0x1F;
indexed:
                ptr = base + index;
                goto fetch;
            }
            ptr = D_8009FA94[0] + 0x20;
fetch:
            return *ptr;
        default:
            if (player) return player->values[id];
            return D_8009F9EC[0]->defaults->values[id];
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
            register struct Defaults *base2 __asm__("$2") = D_8009F9EC[0]->defaults;
            s32 offset;
            __asm__ volatile("" : "=r"(base2) : "0"(base2));
            offset = id * 4;
            return *(s32 *)((char *)base2 + offset + 12);
        }
    } else {
        if (!player) player = D_8009FA58[0]->player;
        return player->a + player->b;
    }
}
