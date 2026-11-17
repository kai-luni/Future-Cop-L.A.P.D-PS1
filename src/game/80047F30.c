// Categories: Graphics > HUD > Race HUD | Graphics > 2D Drawing > Text | Gameplay > Scoring
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
extern s32 D_8009FAA4;
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern State *D_8009FA94;
extern Game *gSinglePlayerEntity __asm__("D_8009F9EC");
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

/**
 * @brief Draws the in-game HUD: health bars for both players, score, the
 *        mission counters, the objective label and the stage line.
 *
 * Confirmed role
 * --------------
 * The function only produces 2D output: two bar calls, five text calls, three
 * rectangles and the marker pass. It is called once per frame from the two
 * gameplay screen updates func_800271A8 and func_80027510 and returns nothing,
 * so it is the whole HUD of a match, split by the player index and by the
 * player count.
 *
 * @param game Game block: health at +0x1C, max health at +0x1E, world position
 *        at +0x50, marker vector at +0xA0, player pointer at +0xAC and an extra
 *        block pointer at +0x134 (that block has its two words at +0xA4/+0xA8).
 *
 * @return Nothing. Three early returns exist: screen state not 4, HUD state not
 *        1, and - inside the first player's layout - "not a one-player game".
 *
 * @par Confirmed gate
 * if (D_8009FA00 != 4) return; the HUD is only drawn in screen state 4. Then
 * func_80047888() and func_8006C080(game) run, and the marker pass
 * func_8005AB8C(game, game->position, game->veca0) plus func_8004EDC8(game)
 * follow.
 *
 * @par Confirmed health bar
 * intensity = (s8)((player+0x58 * player+0x5C + 0x8000) >> 16), clamped at 0 -
 * a Q16 product of the two extra values with rounding. When player+0x326 is
 * positive and bit 0x80 of D_8009FAA4 is set the bar is drawn unbroken with
 * func_80047BE8(12, 206, intensity, 0, 64, 127, player+0x64 < 2); otherwise the
 * ratio (health << 16) / maxhealth colours the bar: red = 127 + ((ratio -
 * (ratio << 6)) >> 16), green = 31 + ((ratio * 96) >> 16), blue = (ratio * 64)
 * >> 16, i.e. green/blue fall as the player loses health while red rises. The
 * last argument is "low on health": +0x64 < 2 for the player, health * 3 <
 * maxhealth for the second game.
 *
 * @par Confirmed mid-screen label
 * When player+0xAE is positive, D_8009F348[player+0xA4] is drawn at y 74: x is
 * 12 normally, and 148 - func_80049340(label) / 2 when player+0xC1 is set, i.e.
 * the label is centred when it belongs to the second player. With two players
 * and no second-player marker a 2 x 480 divider is drawn at 318.
 *
 * @par Confirmed score block
 * The HUD state pointer D_8009FA94 must have mode(+0x5C) == 1. score is
 * state->score[player+0xC1], formatted with D_8009F34C. A flash (colour 9
 * instead of 1 or 3) appears when score >= 30, bit 0x80 of D_8009FAA4 is set and
 * total1(+0x84) + total2(+0x88) < 4. width is 6 for scores of 1000 and more and
 * 0 otherwise - the digit-width compensation of the right-aligned number. x is
 * 152 in a one-player game.
 *
 * The first player's layout (player+0xC1 == 0) draws the box at
 * ((x + 135 - width) * 2, 40) with size ((width + 22) * 2, 20) and the number at
 * x + 138 - width, y 20; the counter text from D_800117C0 with count1(+0x6C) and
 * count2(+0x70) at y 32. In a one-player game the same block is repeated for the
 * second score at y 88/44/56 with count3(+0x74) and count4(+0x78), the stage
 * line D_8009F354 with stage(+0x4C) + 1 at (274, 216) and the second game's
 * health bar at x 295. The second player's layout draws the same box and number
 * at x 6 / y 40 and y 20 instead.
 *
 * @par Likely
 * +0x326 marks a "bar is full / effect active" state and bit 0x80 of D_8009FAA4
 * is a blink phase, since it is the only thing that changes between the two bar
 * variants and suppresses the flash. +0xA4 in the player block indexes a
 * per-objective label table, +0xAE gates its visibility.
 *
 * @par Unknown
 * The unit of the bar intensity (a Q16 product of two extra fields) and the
 * exact meaning of +0x64 (a difficulty or a player index - only the comparison
 * with 2 is visible) cannot be resolved here.
 *
 * @par Globals
 * D_8009FA00 at 0x8009FA00 - screen state, compared with 4.
 * D_8009FA0C at 0x8009FA0C - player count (1 or 2).
 * D_8009FAA4 at 0x8009FAA4 - frame stamp; bit 0x80 is the blink phase.
 * D_8009FA94 at 0x8009FA94 - HUD state: stage +0x4C, mode +0x5C, score at
 * +0x64 (two words), counters +0x6C..+0x78, totals +0x84/+0x88.
 * D_8009F9EC at 0x8009F9EC - second game block (health +0x1C, max +0x1E, extra
 * +0x134 with +0xA4/+0xA8).
 * D_8009F348 at 0x8009F348 - label pointer table.
 * D_8009F34C at 0x8009F34C, D_8009F354 at 0x8009F354, D_800117C0 at
 * 0x800117C0 - format strings for score, stage and counters.
 *
 * @par Control flow
 * 1. Gate on the screen state, then the two setup calls.
 * 2. Health bar (two variants) and the marker pass.
 * 3. Objective label and, with two players, the divider.
 * 4. Gate on HUD mode 1, build the score strings and the flash value.
 * 5. First or second player layout, each with box, score, counters.
 * 6. In a one-player game: second score block, stage line, second bar.
 *
 * @par Calls
 * func_80047888, func_8006C080, func_80047BE8 (three sites), func_8005AB8C,
 * func_8004EDC8, func_80049340 (text width), func_80049100 (text, five sites),
 * func_8001C214 (rectangle, three sites), func_8008CD30 (formatting, four
 * sites).
 *
 * @par Side effects
 * Writes only the local 32-byte text buffer and issues draw commands into the
 * 2D layer; no game state is modified. D_8009F9EC and D_8009FA94 are read, not
 * written.
 *
 * @par Callers
 * func_800271A8 and func_80027510 (analysis/edges.json, kind "direct"); the
 * address also has an "address_taken" edge.
 */
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

    if (gScreenState != 4) return;
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
    if (gPlayerCount >= 2 && !game->player->valuec1)
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
    if (gPlayerCount == 1) x = 152;
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
        if (gPlayerCount != 1) return;
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
        if (!gSinglePlayerEntity) return;
        func_8008CD30(text, D_8009F354, D_8009FA94->stage + 1);
        func_80049100(text, 274, 216, 8);
        other = gSinglePlayerEntity;
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
