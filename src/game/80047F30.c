#include "game/types.h"

typedef struct Player {
    u8 pad00[0x58];
    s32 value58, value5c;
    u8 value60[4];
    u8 value64;
    u8 pad65[0x3f];
    u8 valuea4;
    u8 pada5[9];
    s16 valueae;
    u8 padb0[0x11];
    s8 valuec1;
    u8 padc2[0x264];
    s16 value326;
} Player;
typedef struct Extra {
    u8 pad00[0xa4];
    s32 a4, a8;
} Extra;
typedef struct Game {
    u8 pad00[0x1c];
    s16 health, maxhealth;
    u8 pad20[0x30];
    u8 position[0x50];
    u8 veca0[0xc];
    Player *player;
    u8 padb0[0x84];
    Extra *extra;
} Game;
typedef struct State {
    u8 pad00[0x4c];
    s32 stage;
    u8 pad50[0xc];
    s32 mode;
    u8 pad60[4];
    s32 score[2];
    s32 count1, count2, count3, count4;
    u8 pad7c[8];
    s32 total1, total2;
} State;
extern s32 D_8009FA00, D_8009FA0C, D_8009FAA4;
extern State *D_8009FA94;
extern Game *D_8009F9EC;
extern u8 D_8009F34C[], D_8009F354[], D_800117C0[];
extern u8 **D_8009F348;
extern void func_80047888(void);
extern void func_8006C080(Game *);
extern void func_80047BE8(s32, s32, s32, s32, s32, s32, s32);
extern void func_8005AB8C(Game *, void *, void *);
extern void func_8004EDC8(Game *);
extern s32 func_80049340(u8 *);
extern void func_80049100(u8 *, s32, s32, s32);
extern void func_8001C214(u32, s32, s32, s32, s32, s32);
extern void func_8008CD30(void *, void *, ...);

void func_80047F30(Game *game)
{
    u8 text[32];
    register Player *player __asm__("$8");
    Player *p2;
    s32 intensity, score, flash, width, color, label_x;
    register s32 x __asm__("$18");
    register s32 ratio __asm__("$7");
    register State *state __asm__("$6");
    u8 *label;
    Game *other;

    if (D_8009FA00 != 4) return;
    func_80047888();
    func_8006C080(game);
    player = game->player;
    intensity = (s8)((player->value58 * player->value5c + 0x8000) >> 16);
    if (intensity < 0) intensity = 0;
    if (player->value326 > 0 && (D_8009FAA4 & 0x80)) {
        func_80047BE8(12, 206, intensity, 0, 64, 127, player->value64 < 2);
    } else {
        ratio = ((game->health - ((game->health >> 31) & game->health)) << 16) / game->maxhealth;
        func_80047BE8(15, 206, intensity, 127 + ((ratio - (ratio << 6)) >> 16),
                     31 + ((ratio * 96) >> 16), (ratio * 64) >> 16, player->value64 < 2);
    }
    func_8005AB8C(game, game->position, game->veca0);
    func_8004EDC8(game);
    p2 = game->player;
    if (p2->valueae > 0) {
        label = D_8009F348[p2->valuea4];
        label_x = 12;
        if (p2->valuec1) label_x = 148 - func_80049340(label) / 2;
        func_80049100(label, label_x, 74, 8);
    }
    if (D_8009FA0C >= 2 && !game->player->valuec1)
        func_8001C214(1, 318, 0, 2, 480, 0);
    state = D_8009FA94;
    if (state->mode != 1) return;
    score = state->score[game->player->valuec1];
    func_8008CD30(text, D_8009F34C, score);
    flash = 0;
    if (score >= 30 && (D_8009FAA4 & 0x80)) {
        register s32 a __asm__("$3") = D_8009FA94->total1;
        register s32 b __asm__("$2") = D_8009FA94->total2;
        __asm__ volatile("" : "+r"(a), "+r"(b));
        a += b;
        __asm__ volatile("" : "+r"(a));
        flash = a < 4;
    }
    width = 0;
    if (score >= 1000) width = 6;
    x = 0;
    if (D_8009FA0C == 1) x = 152;
    if (!game->player->valuec1) {
        {
            register s32 arg __asm__("$4") = 1;
            register s32 px __asm__("$5") = x + 135;
            __asm__ volatile("" : "+r"(arg), "+r"(px));
            func_8001C214(arg, (px - width) * 2, 40, (width + 22) * 2, 20, 1);
        }
        {
            register s32 c __asm__("$7") = 1;
            register s32 px __asm__("$2") = x + 138;
            __asm__ volatile("" : "+r"(c), "+r"(px));
            {
                register s32 arg __asm__("$5") = px - width;
                if (flash) c = 9;
                func_80049100(text, arg, 20, c);
            }
        }
        func_8008CD30(text, D_800117C0, D_8009FA94->count1, D_8009FA94->count2);
        func_8001C214(1, (x + 127) * 2, 64, 60, 20, 1);
        func_80049100(text, x + 130, 32, 1);
        if (D_8009FA0C != 1) return;
        {
        register s32 one __asm__("$16");
        register s32 height __asm__("$20");
        score = D_8009FA94->score[1];
        func_8008CD30(text, D_8009F34C, score);
        flash = 0;
        if (score >= 30 && (D_8009FAA4 & 0x80)) {
            register s32 a __asm__("$3") = D_8009FA94->total1;
            register s32 b __asm__("$2") = D_8009FA94->total2;
            __asm__ volatile("" : "+r"(a), "+r"(b));
            a += b;
            __asm__ volatile("" : "+r"(a));
            flash = a < 4;
        }
        width = 0;
        if (score >= 1000) width = 6;
        {
            register s32 arg __asm__("$4") = 1;
            register s32 px __asm__("$5") = x + 135;
            register s32 y __asm__("$6");
            register s32 w __asm__("$7");
            __asm__ volatile("" : "+r"(arg), "+r"(px));
            px = (px - width) * 2;
            y = 88;
            w = (width + 22) * 2;
            height = 20;
            __asm__ volatile("addu %0,%1,$zero" : "=r"(one) : "r"(arg), "r"(px), "r"(y), "r"(w), "r"(height));
            func_8001C214(arg, px, y, w, height, one);
        }
        {
            register s32 c __asm__("$7") = 3;
            register s32 px __asm__("$2") = x + 138;
            __asm__ volatile("" : "+r"(c), "+r"(px));
            {
                register s32 arg __asm__("$5") = px - width;
                if (flash) c = 9;
                func_80049100(text, arg, 44, c);
            }
        }
        func_8008CD30(text, D_800117C0, D_8009FA94->count3, D_8009FA94->count4);
        func_8001C214(1, (x + 127) * 2, 112, 60, height, one);
        }
        func_80049100(text, x + 130, 56, 3);
        if (!D_8009F9EC) return;
        func_8008CD30(text, D_8009F354, D_8009FA94->stage + 1);
        func_80049100(text, 274, 216, 8);
        other = D_8009F9EC;
        if (other->health > 0) {
            ratio = (other->health << 16) / other->maxhealth;
            intensity = (s8)((other->extra->a4 * other->extra->a8 + 0x8000) >> 16);
            func_80047BE8(295, 206, intensity, 127 + ((ratio - (ratio << 6)) >> 16),
                         31 + ((ratio * 96) >> 16), (ratio * 64) >> 16,
                         other->health * 3 < other->maxhealth);
        }
    } else {
        func_8001C214(1, 6, 40, (width + 22) * 2, 20, 1);
        color = 3;
        if (flash) color = 9;
        func_80049100(text, 6, 20, color);
        func_8008CD30(text, D_800117C0, D_8009FA94->count3, D_8009FA94->count4);
        func_8001C214(1, 6, 64, 60, 20, 1);
        func_80049100(text, 6, 32, 3);
    }
}
