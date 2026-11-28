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

/* A 0x1C-byte bank record; 0x6E is an extra guard slot with a nonzero count. */
struct SoundBank {
    s32 samples[4];
    u8 count, cursor, voices, priority;
    u8 pad[4];
    s32 head;
};
struct SoundBank soundBanks[0x6F] __asm__("D_800A7938");
volatile u8 soundEnabled __asm__("D_8009F340");

static const char attenuationCall[] = "func_800458E8";
static const char allocCall[] = "func_80046B04";
static const char panCall[] = "func_800460D4";
static const char startCall[] = "func_8004772C";
static const char releaseCall[] = "func_80046698";

static struct SoundFixture {
    s32 audible, allocated;
    u8 started;
    s8 *mix;
    s32 allocIndex, startVolume, startExtra, panInput;
} sound;

s32 mockAttenuation(s32 x, s32 z, s8 *mix, void *object)
    __asm__("func_800458E8");
s32 mockAttenuation(s32 x, s32 z, s8 *mix, void *object)
{
    fck_record(attenuationCall, x, z, (s32)object);
    sound.mix = mix;
    mix[0] = -22;
    mix[1] = 15;
    return sound.audible;
}

s32 mockAlloc(s32 volume, s32 priority, s32 voiceClass, u32 index)
    __asm__("func_80046B04");
s32 mockAlloc(s32 volume, s32 priority, s32 voiceClass, u32 index)
{
    fck_record(allocCall, volume, priority, voiceClass);
    sound.allocIndex = index;
    return sound.allocated;
}

void mockPan(s8 *mix) __asm__("func_800460D4");
void mockPan(s8 *mix)
{
    fck_record(panCall, mix == sound.mix, 0, 0);
    sound.panInput = mix[1];
    mix[0] = 47;
    mix[1] = -31;
}

u8 mockStart(struct SoundBank *bank, s32 handle, s32 pan, s32 volume, s32 extra)
    __asm__("func_8004772C");
u8 mockStart(struct SoundBank *bank, s32 handle, s32 pan, s32 volume, s32 extra)
{
    fck_record(startCall, (s32)bank, handle, pan);
    sound.startVolume = volume;
    sound.startExtra = extra;
    return sound.started;
}

void mockRelease(s32 handle) __asm__("func_80046698");
void mockRelease(s32 handle)
{
    fck_record(releaseCall, handle, 0, 0);
}

static void expectArgs(int step, s32 a0, s32 a1, s32 a2)
{
    FCK_EQ(fck_log[step].a0, a0, "recorded call arg 0");
    FCK_EQ(fck_log[step].a1, a1, "recorded call arg 1");
    FCK_EQ(fck_log[step].a2, a2, "recorded call arg 2");
}

static struct SoundBank banksBefore[0x6F];

static void primeSound(int slot, int count, int audible, int handle, int started)
{
    lk_fill(soundBanks, 0x96, sizeof(soundBanks));
    soundBanks[slot].count = count;
    soundBanks[slot].priority = 0xD4;
    memcpy(banksBefore, soundBanks, sizeof(soundBanks));
    sound.audible = audible;
    sound.allocated = handle;
    sound.started = started;
    fck_log_reset();
}

static void expectAllocated(int step, s32 voiceClass, int slot)
{
    expectArgs(step, -22, 0xD4, voiceClass);
    FCK_EQ(sound.allocIndex, slot, "allocator receives the selected bank index");
}

static void expectStarted(int step, int slot, s32 handle, s32 extra)
{
    FCK_EQ(fck_log[step - 1].a0, 1, "pan gets the same stack mix as attenuation");
    FCK_EQ(sound.panInput, 15, "pan observes attenuation's initial mix");
    expectArgs(step, (s32)&soundBanks[slot], handle, -31);
    FCK_EQ(sound.startVolume, 47, "starter reads post-pan volume");
    FCK_EQ(sound.startExtra, extra, "starter extra argument");
}

extern s32 playSpatialBank(s32 x, s32 z, u32 slot, s32 object, s32 voiceClass)
    __asm__("func_80046F9C");

static void test_80046F9C(void)
{
    static const char *order[] = { attenuationCall, allocCall, panCall, startCall, releaseCall };
    static const struct {
        u32 slot;
        u8 enabled, count, audible, started;
        s32 allocation, result;
        int calls;
    } cases[] = {
        {0x6D, 0, 2, 1, 1, 0x9876, 0, 0},
        {0x6E, 2, 2, 1, 1, 0x9876, 0, 0},
        {0x6D, 2, 0, 1, 1, 0x9876, 0, 0},
        {0x6D, 2, 2, 0, 1, 0x9876, 0, 1},
        {0x6D, 2, 2, 1, 1, 0,      0, 2},
        {0x6D, 2, 2, 1, 1, 0x9876, 0x9876, 4},
        {0x6D, 2, 2, 1, 0, 0x9876, 0, 5}
    };
    int i;

    /* @leaf 80046F9C source gPlaySpatialSoundBankSlot.c:62-90 sound flag, exclusive slot bound and count gate; attenuation then signed-class allocation, pan before start with -1 extra, release only on start failure */
    LK_FUNC("func_80046F9C");
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        const int calls = cases[i].calls;
        const u32 slot = cases[i].slot;
        s32 ret;

        soundEnabled = cases[i].enabled;
        primeSound(slot, cases[i].count, cases[i].audible, cases[i].allocation, cases[i].started);
        ret = playSpatialBank(-91, 23, slot, (s32)&session, 0x1FE);

        FCK_EQ(ret, cases[i].result, "spatial bank voice result");
        fck_expect_order(order, calls, "spatial bank call sequence");
        if (calls >= 1)
            expectArgs(0, -91, 23, (s32)&session);
        if (calls >= 2)
            expectAllocated(1, -2, slot);
        if (calls >= 4)
            expectStarted(3, slot, 0x9876, -1);
        if (calls == 5)
            expectArgs(4, 0x9876, 0, 0);
        FCK_EQ(soundEnabled, cases[i].enabled, "sound enable byte untouched");
        FCK_EQ(memcmp(soundBanks, banksBefore, sizeof(soundBanks)), 0,
               "caller does not write bank records");
    }
}

struct LoadoutList {
    u8 *entries;
    s32 unused;
    u8 count, min, max, pad[9];
};

struct MissionLimits {
    u8 pad[0x19];
    u8 primaryMax, heavyMax, specialMax;
    u8 tail[8];
};

struct LoadoutList loadout[4] __asm__("D_8009C0A8");
/* These are interior retail aliases, not independent minima. */
__asm__(".globl D_8009C0C5\n.set D_8009C0C5,D_8009C0A8+0x1D\n"
        ".globl D_8009C0D9\n.set D_8009C0D9,D_8009C0A8+0x31");
u8 alternateWeapons[4] __asm__("D_8009C090");
u8 defaultWeapons[5] __asm__("D_8009C070");
u8 loadoutBackdrop[2] __asm__("D_800A362B");
s8 primaryIndices[3] __asm__("D_8009F924");
s8 heavyIndices[3] __asm__("D_8009F926");
s8 specialIndices[3] __asm__("D_8009F928");
s8 selectedMission __asm__("D_8009F880");
static s8 missionMap[] = {1, 2, 0};
s8 *missionRows __asm__("D_8009F884") = missionMap;
struct MissionLimits missionLimits[3] __asm__("D_8009B4E4");
u16 loadoutItems[0x45] __asm__("D_8009B7D0");

static const char lookupCall[] = "func_8007D39C";
static const u8 lookupResults[] = {0xFE, 8, 9, 0, 2, 0xFF};

u8 mockLookup(struct LoadoutList *list, u8 value) __asm__("func_8007D39C");
u8 mockLookup(struct LoadoutList *list, u8 value)
{
    int call = fck_log_count;
    fck_record(lookupCall, (s32)list, value, list->count);
    if (call < sizeof(lookupResults) / sizeof(lookupResults[0]))
        return lookupResults[call];
    return 0;
}

extern void initSelectionIndices(void) __asm__("func_8007D408");

static void test_8007D408(void)
{
    static const char *order[] = {
        lookupCall, lookupCall, lookupCall, lookupCall, lookupCall, lookupCall
    };
    static const struct {
        s32 mode, x, y, z;
        u8 count, backdrop, primaryMin, heavyMin, specialMin;
    } cases[] = {
        {1, 0, 2, 0, 4, 7, 1, 0, 1},
        {2, 1, 0, -1, 5, 2, 0, 1, 0}
    };
    struct LoadoutList expected[4];
    struct Session before;
    struct MissionLimits limitsBefore[3];
    int i;

    /* @leaf 8007D408 source gInitSelectionIndices.c:53-88 configures list before six ordered lookups, takes maxima from the mapped mission row, clamps signed indices using three unlock flags, then resets flags and halfwords */
    LK_FUNC("func_8007D408");
    for (i = 0; i < 2; i++) {
        lk_fill(&session, 0x89, sizeof(session));
        session.mode = cases[i].mode;
        session.primaryUnlocked = cases[i].x;
        session.heavyUnlocked = cases[i].y;
        session.specialUnlocked = cases[i].z;
        session.primary0 = 0xE1;
        session.primary1 = 11;
        session.heavy0 = 22;
        session.heavy1 = 33;
        session.special0 = 44;
        session.special1 = 55;
        before = session;

        lk_fill(loadout, 0xA5, sizeof(loadout));
        loadout[0].count = 6;
        selectedMission = 1;
        lk_fill(missionLimits, 0xB6, sizeof(missionLimits));
        missionLimits[2].primaryMax = 3;
        missionLimits[2].heavyMax = 4;
        missionLimits[2].specialMax = 1 + i * 2;
        memcpy(limitsBefore, missionLimits, sizeof(missionLimits));
        memcpy(expected, loadout, sizeof(loadout));
        expected[0].min = cases[i].primaryMin;
        expected[0].max = 3;
        expected[1].count = cases[i].count;
        expected[1].min = cases[i].heavyMin;
        expected[1].max = 4;
        expected[2].entries = i == 0 ? alternateWeapons : defaultWeapons;
        expected[2].count = cases[i].count;
        expected[2].min = cases[i].specialMin;
        expected[2].max = i == 0 ? 1 : 3;
        lk_fill(loadoutItems, 0xC7, sizeof(loadoutItems));
        lk_fill(loadoutBackdrop, 0xD8, sizeof(loadoutBackdrop));
        lk_fill(primaryIndices, 0xE9, sizeof(primaryIndices));
        lk_fill(heavyIndices, 0xE9, sizeof(heavyIndices));
        lk_fill(specialIndices, 0xE9, sizeof(specialIndices));
        ready1 = 17;
        ready2 = 19;
        fck_log_reset();

        initSelectionIndices();

        fck_expect_order(order, 6, "six lookup calls");
        expectArgs(0, (s32)&loadout[0], 0xE1, 6);
        expectArgs(1, (s32)&loadout[0], 11, 6);
        expectArgs(2, (s32)&loadout[1], 22, cases[i].count);
        expectArgs(3, (s32)&loadout[1], 33, cases[i].count);
        expectArgs(4, (s32)&loadout[2], 44, cases[i].count);
        expectArgs(5, (s32)&loadout[2], 55, cases[i].count);
        FCK_EQ(memcmp(loadout, expected, sizeof(loadout)), 0,
               "list entries, counts, limits and untouched bytes");
        FCK_EQ(primaryIndices[0], cases[i].primaryMin, "primary signed index clamps up");
        FCK_EQ(primaryIndices[1], 3, "primary index clamps down");
        FCK_EQ(heavyIndices[0], 4, "heavy index clamps down");
        FCK_EQ(heavyIndices[1], cases[i].heavyMin, "heavy index respects minimum");
        FCK_EQ(specialIndices[0], i == 0 ? 1 : 2, "special upper clamp or interior value");
        FCK_EQ(specialIndices[1], cases[i].specialMin, "special signed index clamps up");
        FCK_CHECK((u8)primaryIndices[2] == 0xE9 && (u8)heavyIndices[2] == 0xE9 &&
                  (u8)specialIndices[2] == 0xE9, "index array neighbors untouched");
        FCK_EQ(loadoutBackdrop[0], cases[i].backdrop, "mode selects loadout backdrop");
        FCK_EQ(loadoutBackdrop[1], 0xD8, "backdrop neighbor untouched");
        FCK_EQ(ready1, 0, "loadout clears first ready flag");
        FCK_EQ(ready2, 0, "loadout clears second ready flag");
        FCK_EQ(loadoutItems[0x16 / 2], 100, "loadout item +0x16 set to 100");
        FCK_EQ(loadoutItems[0x86 / 2], 100, "loadout item +0x86 set to 100");
        FCK_CHECK(lk_all(loadoutItems, 0, 0x16, 0xC7) &&
                  lk_all(loadoutItems, 0x18, 0x86, 0xC7) &&
                  lk_all(loadoutItems, 0x88, sizeof(loadoutItems), 0xC7),
                  "other loadout item bytes untouched");
        FCK_EQ(memcmp(&session, &before, sizeof(session)), 0, "session is read-only");
        FCK_EQ(memcmp(missionLimits, limitsBefore, sizeof(missionLimits)), 0,
               "mission limit rows are read-only");
        FCK_EQ(selectedMission, 1, "selected mission untouched");
        FCK_EQ(missionRows, missionMap, "mission map binding untouched");
    }
}

u8 soundIds[0x47] __asm__("D_800A1830");
static const char resolveCall[] = "func_80045494";
static s32 resolvedSlot;

s32 mockResolve(s32 id, u8 *table, s32 limit) __asm__("func_80045494");
s32 mockResolve(s32 id, u8 *table, s32 limit)
{
    fck_record(resolveCall, id, (s32)table, limit);
    return resolvedSlot;
}

extern s32 playSoundId(s32 track, s32 x, s32 z, s32 extra, s32 handle, void *object)
    __asm__("func_80046E88");

static void test_80046E88(void)
{
    static const char *newOrder[] = {
        resolveCall, attenuationCall, allocCall, panCall, startCall, releaseCall
    };
    static const char *existingOrder[] = {
        resolveCall, attenuationCall, panCall, startCall
    };
    static const struct {
        s32 slot, supplied, allocation, result;
        u8 audible, started;
        int calls;
    } cases[] = {
        {0xFF, 0,      0xA321, 0,      1, 1, 1},
        {12,   0,      0xA321, 0,      0, 1, 2},
        {12,   0,      0,      0,      1, 1, 3},
        {12,   0,      0xA321, 0xA321, 1, 1, 5},
        {12,   0,      0xA321, 0,      1, 0, 6},
        {12,   0xC123, 0xA321, 0,      1, 1, 4},
        {12,   0xC123, 0xA321, 0,      1, 0, 4}
    };
    int i;

    /* @leaf 80046E88 source gPlaySoundIdOnVoice.c:59-87 resolves ID with limit 0x47, rejects sentinel or inaudible results, allocates only for null supplied handle, passes caller extra to start, releases only a newly allocated failed voice */
    LK_FUNC("func_80046E88");
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        const int calls = cases[i].calls;
        const int existing = cases[i].supplied != 0;
        s32 ret;

        /* Unlike 80046F9C, this caller has no enabled/count preflight. */
        soundEnabled = 0;
        primeSound(12, 0, cases[i].audible, cases[i].allocation, cases[i].started);
        lk_fill(soundIds, 0xDA, sizeof(soundIds));
        resolvedSlot = cases[i].slot;
        ret = playSoundId(54, -89, 99, -27, cases[i].supplied, &session);

        FCK_EQ(ret, cases[i].result, "only a successful new allocation returns a handle");
        fck_expect_order(existing ? existingOrder : newOrder, calls, "sound-ID call sequence");
        expectArgs(0, 54, (s32)soundIds, 0x47);
        if (calls >= 2)
            expectArgs(1, -89, 99, (s32)&session);
        if (!existing && calls >= 3)
            expectAllocated(2, 0, 12);
        if (existing)
            expectStarted(3, 12, 0xC123, -27);
        if (calls >= 5)
            expectStarted(4, 12, 0xA321, -27);
        if (calls == 6)
            expectArgs(5, 0xA321, 0, 0);
        FCK_CHECK(lk_all(soundIds, 0, sizeof(soundIds), 0xDA), "ID table untouched");
        FCK_EQ(soundEnabled, 0, "sound enable byte untouched");
        FCK_EQ(memcmp(soundBanks, banksBefore, sizeof(soundBanks)), 0,
               "caller does not write bank records");
    }
}

int main(void)
{
    test_8007B004();
    test_80046F9C();
    test_8007D408();
    test_80046E88();
    return lk_done("leaf_boot_r4");
}
