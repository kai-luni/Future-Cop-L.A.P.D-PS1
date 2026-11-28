/* Boot/front-end round 4: unchanged original units, linked under their retail
 * labels. The fixture owns the RAM labels; audio dependencies are scripted and
 * recorded to isolate each caller from the audio engine. */
#include "leaf_kit.h"

struct Session {
    u8 pad00[8];
    s32 selection, players;
    u8 pad10[0x18];
    u8 primary0, pad29[3], heavy0, pad2D[3], special0, pad31[3];
    u8 primary1, pad35[3], heavy1, pad39[3], special1, pad3D[0x1F];
    s32 mode;
    u8 pad60[0x2C];
    s32 primaryUnlocked, heavyUnlocked, specialUnlocked, difficulty;
};

static struct Session session;
struct Session *sessions[2] __asm__("D_8009FA94") = { &session, 0 };
s32 ready1 __asm__("D_8009F8E0");
s32 ready2 __asm__("D_8009F8E4");

struct MenuNode {
    u8 pad[0x24];
    struct MenuNode *next;
};

static struct MenuNode menuHead, menuTail;
struct MenuNode *fontBank __asm__("D_8009F468") = &menuHead;
struct MenuNode transition __asm__("D_800A3688");
s32 playerSelection __asm__("D_8009F8F0");
s32 stagedDifficulty __asm__("D_8009F8F4");
u16 missionItems[0x45] __asm__("D_8009BB34");
u8 onePlayerBackdrop[2] __asm__("D_800A37AB");
u8 otherBackdrop[2] __asm__("D_800A37DB");
extern void initMissionSelect(void) __asm__("func_8007B004");

static void test_8007B004(void)
{
    static const struct {
        s32 mode, players;
        u8 onePlayer, other;
        int unlink;
    } cases[] = {
        {1, 1, 9, 0xB6, 1},
        {1, 2, 0xA5, 3, 0},
        {2, 1, 1, 0xB6, 0},
        {0, 0, 0xA5, 8, 1}
    };
    struct Session before;
    int i;

    /* @leaf 8007B004 source gInitMissionSelectState.c:58-91 copies session words, resets ready flags and two halfwords, bypasses only the transition successor, sets one backdrop byte by exact equality */
    LK_FUNC("func_8007B004");
    for (i = 0; i < 4; i++) {
        lk_fill(&session, 0x64, sizeof(session));
        session.mode = cases[i].mode;
        session.players = cases[i].players;
        session.selection = -4;
        session.difficulty = 0x12345678;
        before = session;
        lk_fill(&menuHead, 0xBA, sizeof(menuHead));
        menuHead.next = cases[i].unlink ? &transition : &menuTail;
        transition.next = &menuTail;
        menuTail.next = &transition;
        ready1 = 3;
        ready2 = 4;
        playerSelection = 5;
        stagedDifficulty = 6;
        lk_fill(missionItems, 0xCD, sizeof(missionItems));
        lk_fill(onePlayerBackdrop, 0xA5, sizeof(onePlayerBackdrop));
        lk_fill(otherBackdrop, 0xB6, sizeof(otherBackdrop));
        fck_log_reset();

        initMissionSelect();

        FCK_EQ(playerSelection, -4, "copies session +0x08");
        FCK_EQ(stagedDifficulty, 0x12345678, "copies session +0x98");
        FCK_EQ(ready1, 0, "clears player 1 ready flag");
        FCK_EQ(ready2, 0, "clears player 2 ready flag");
        FCK_EQ(missionItems[0x32 / 2], 100, "sets halfword +0x32");
        FCK_EQ(missionItems[0x86 / 2], 100, "sets halfword +0x86");
        FCK_CHECK(lk_all(missionItems, 0, 0x32, 0xCD) &&
                  lk_all(missionItems, 0x34, 0x86, 0xCD) &&
                  lk_all(missionItems, 0x88, sizeof(missionItems), 0xCD),
                  "other mission item bytes untouched");
        FCK_EQ(menuHead.next, &menuTail, "bypasses transition or preserves other link");
        FCK_EQ(transition.next, &menuTail, "transition successor untouched");
        FCK_EQ(menuTail.next, &transition, "nonmatching successor not chased");
        FCK_CHECK(lk_all(menuHead.pad, 0, sizeof(menuHead.pad), 0xBA),
                  "other head bytes untouched");
        FCK_EQ(onePlayerBackdrop[0], cases[i].onePlayer, "one-player backdrop truth table");
        FCK_EQ(otherBackdrop[0], cases[i].other, "other backdrop truth table");
        FCK_EQ(onePlayerBackdrop[1], 0xA5, "one-player neighbor untouched");
        FCK_EQ(otherBackdrop[1], 0xB6, "other backdrop neighbor untouched");
        FCK_EQ(memcmp(&session, &before, sizeof(session)), 0, "session is read-only");
        FCK_EQ(fck_log_count, 0, "no recorded dependencies");
    }
}

int main(void)
{
    test_8007B004();
    return lk_done("leaf_boot_r4");
}
