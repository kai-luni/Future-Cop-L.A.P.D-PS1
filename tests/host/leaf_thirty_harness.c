/* Source characterizations of unchanged src/game units on synthetic RAM.
 * Layouts and signatures are taken from the cited units. For void functions
 * the observable result is the write set or callback, not a return register.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Real dependencies already covered by other suites. The generated runner
 * links only marked units, so compile these here without claiming coverage.
 * The relative paths also work in the mutation tool's source snapshot.
 */
#include "../../src/game/other/gFillMemory.c"
#include "../../src/game/boot-frontend/gGetPadType.c"
#include "../../src/game/memory-resources/gRecycleModuleReadBuffer.c"
#include "../../src/game/renderer-scene/gReleasePoolNode.c"
#include "../../src/game/gpu-2d-hud/gLookupFontGlyph.c"
#include "../../src/game/utilities/gComputeDualChecksum.c"

struct DmaEntry { u8 before[12]; s32 completion; u8 after[8]; };
struct DmaEntry dma_entries[4] __asm__("D_800A8EA8");
s32 dma_initialized __asm__("D_8009F9C8");
s32 dma_read_index __asm__("D_8009FA80");
s32 dma_write_index __asm__("D_8009F998");
s32 dma_active __asm__("D_8009FA34");
void gSpuCancelDmaRequests(void) __asm__("func_8002129C");

static void test_dma_queue(void)
{
    /* @leaf 8002129C source src/game/platform/gSpuCancelDmaRequests.c: initialized queue cancels only the selected completion */
    LK_FUNC("func_8002129C");
    dma_initialized = 0;
    dma_read_index = 2;
    dma_write_index = 3;
    dma_active = 0;
    lk_fill(dma_entries, 0xA5, sizeof(dma_entries));
    gSpuCancelDmaRequests();
    FCK_EQ(dma_write_index, 3, "uninitialized queue retains writer");
    FCK_CHECK(lk_all(dma_entries, 0, sizeof(dma_entries), 0xA5), "uninitialized queue untouched");

    dma_initialized = -1;
    dma_active = 1; /* stays active: exercise bounded exit without a DMA device */
    gSpuCancelDmaRequests();
    FCK_EQ(dma_write_index, 2, "writer collapses to reader");
    FCK_EQ(dma_entries[2].completion, 0, "selected completion removed");
    FCK_CHECK(lk_all(dma_entries, 0, 2 * sizeof(struct DmaEntry) + 12, 0xA5),
              "queue bytes before the completion unchanged");
    FCK_CHECK(lk_all(dma_entries, 2 * sizeof(struct DmaEntry) + 16, sizeof(dma_entries), 0xA5),
              "queue bytes after the completion unchanged");
    FCK_EQ(dma_read_index, 2, "reader unchanged");
    FCK_EQ(dma_active, 1, "request cancellation does not force DMA inactive");
}

struct UnkC14C {
    s32 elapsed;
    s16 limit;
    u8 selector, option, pad08[6], kind, phase, step, secondary, pad12[2];
    u16 acceleration, speed, tuning, duration;
    s32 accumulators[6], basis[9], trailing[3];
};
struct UnkC14CSrc {
    u8 pad00[0x15], selector, secondary, pad17[7], kind, pad1F;
    u32 speed, limit;
    u8 option, pad29;
    s16 acceleration;
    u16 tuning;
    u8 pad2E[4];
    u16 duration;
};
s32 motion_basis[9] __asm__("D_8009AA38");
void gInitSpawnMotionState(struct UnkC14C *dest, struct UnkC14CSrc *src) __asm__("func_8003C14C");

static void test_motion(void)
{
    struct UnkC14C dest;
    struct UnkC14CSrc src;
    int i;

    /* @leaf 8003C14C source src/game/gameplay-ai/gInitSpawnMotionState.c: tuning conversion and selector-controlled signed limit */
    LK_FUNC("func_8003C14C");
    lk_fill(&src, 0x6D, sizeof(src));
    lk_fill(&dest, 0xA5, sizeof(dest));
    for (i = 0; i < 9; i++) motion_basis[i] = -31 + 7 * i;
    src.selector = 0;
    src.secondary = 0xF5;
    src.kind = 0xC8;
    src.option = 0x87;
    src.speed = 0xC000;         /* 3 * 75 after shifting by 14 */
    src.limit = 0x800000;      /* 512 * 75 becomes negative in s16 */
    src.acceleration = -2048;  /* low halfword of the shifted product is -150 */
    src.tuning = 0xB123;
    src.duration = 0xFE01;
    gInitSpawnMotionState(&dest, &src);
    FCK_EQ(dest.limit, -27136, "no selector retains negative signed limit");
    FCK_EQ(dest.speed, 225, "speed conversion");
    FCK_EQ(dest.acceleration, 65386, "negative acceleration conversion");
    FCK_CHECK(dest.selector == 0 && dest.secondary == 0xF5 && dest.kind == 0xC8 &&
              dest.option == 0x87 && dest.tuning == 0xB123 && dest.duration == 0xFE01,
              "source fields routed to their destination slots");
    for (i = 0; i < 9; i++) FCK_EQ(dest.basis[i], motion_basis[i], "basis word copied in order");
    FCK_CHECK(dest.elapsed == 0 && dest.phase == 0 && dest.step == 0, "scalar work state reset");
    FCK_CHECK(dest.accumulators[0] == 0 && lk_all(&dest, 0x24, 0x34, 0) &&
              lk_all(&dest, 0x58, 0x64, 0), "accumulators reset");
    FCK_CHECK(lk_all(&dest, 8, 0xE, 0xA5) && lk_all(&dest, 0x12, 0x14, 0xA5) &&
              lk_all(&dest, 0x20, 0x24, 0xA5), "destination gaps untouched");

    src.selector = 2;
    gInitSpawnMotionState(&dest, &src);
    FCK_EQ(dest.limit, 0, "nonzero selector clamps negative limit");
    src.limit = 0x8000;
    gInitSpawnMotionState(&dest, &src);
    FCK_EQ(dest.limit, 150, "positive limit preserved with selector set");
}

struct SoundChannel {
    s32 id, priority;
    s16 slot;
    s8 index;
    u8 unk0B;
    s32 unk0C, unk10;
    u8 pad14[0x14];
    s32 unk28;
    struct SoundChannel *next, *prev;
};
struct SoundVoice {
    s32 bank;
    u8 pad04[0x20];
    struct SoundVoice *next, *prev;
};
struct SoundBank {
    s32 samples[4];
    u8 count, cursor, voices, pad13[5];
    struct SoundVoice *head;
};
struct SoundChannel channels[31] __asm__("D_800A1160");
s32 channel_count __asm__("D_8009F720");
struct SoundChannel *channel_head __asm__("D_8009F724");
struct SoundChannel *queue_head __asm__("D_8009F728");
struct SoundChannel *queue_tail __asm__("D_8009F72C");
struct SoundBank banks[2] __asm__("D_800A7938");
void gInitSoundChannelList(void) __asm__("func_8004582C");
void gUnlinkSoundBankVoice(struct SoundVoice *voice) __asm__("func_80046804");

static void test_sound_lists(void)
{
    struct SoundVoice voice[3];
    int i;

    /* @leaf 8004582C source src/game/boot-frontend/gInitSoundChannelList.c: exact channel count and singly linked free list */
    LK_FUNC("func_8004582C");
    lk_fill(channels, 0xA5, sizeof(channels));
    channel_count = -9;
    channel_head = channels + 28;
    queue_head = channels + 29;
    queue_tail = channels + 30;
    gInitSoundChannelList();
    FCK_CHECK(channel_head == channels && queue_head == 0 && queue_tail == 0, "head installed and queue reset");
    FCK_EQ(channel_count, 0, "no allocated channels");
    for (i = 0; i < 30; i++) {
        FCK_EQ(channels[i].id, -1, "channel unused");
        FCK_EQ(channels[i].index, i, "channel index matches slot");
        FCK_CHECK(channels[i].next == (i == 29 ? 0 : channels + i + 1), "next points to successor or null");
        FCK_CHECK(lk_all(channels + i, 4, 0xA, 0xA5) &&
                  lk_all(channels + i, 0xB, 0x2C, 0xA5) &&
                  lk_all(channels + i, 0x30, 0x34, 0xA5), "unrelated channel fields unchanged");
    }
    FCK_CHECK(lk_all(channels + 30, 0, sizeof(channels[0]), 0xA5), "backed slot beyond 30 untouched");

    /* @leaf 80046804 source src/game/platform/gUnlinkSoundBankVoice.c: remove middle, head and last voice from its bank */
    LK_FUNC("func_80046804");
    lk_fill(banks, 0xA5, sizeof(banks));
    lk_fill(voice, 0x6D, sizeof(voice));
    for (i = 0; i < 3; i++) voice[i].bank = 1;
    banks[1].head = voice;
    banks[1].voices = 3;
    voice[0].prev = voice[2].next = 0;
    voice[0].next = voice[2].prev = voice + 1;
    voice[1].prev = voice;
    voice[1].next = voice + 2;
    gUnlinkSoundBankVoice(voice + 1);
    FCK_CHECK(banks[1].head == voice && voice[0].next == voice + 2 && voice[2].prev == voice,
              "middle unlink bridges neighbors without replacing head");
    FCK_CHECK(voice[1].prev == voice && voice[1].next == voice + 2, "removed node links not cleared");
    FCK_EQ(banks[1].voices, 2, "middle unlink decrements count");
    gUnlinkSoundBankVoice(voice);
    FCK_CHECK(banks[1].head == voice + 2 && voice[2].prev == 0, "head unlink updates successor and bank");
    FCK_EQ(banks[1].voices, 1, "head unlink decrements count");
    gUnlinkSoundBankVoice(voice + 2);
    FCK_CHECK(banks[1].head == 0 && banks[1].voices == 0, "last unlink empties the bank");
    FCK_CHECK(lk_all(banks, 0, sizeof(banks[0]), 0xA5), "other bank unaffected");
    FCK_CHECK(lk_all(&banks[1], 0, 0x12, 0xA5) && lk_all(&banks[1], 0x13, 0x18, 0xA5),
              "sample data and unrelated bank fields unchanged");
}

struct ScriptTimer { s32 count, period, remaining; u8 *trigger, *action; };
struct Entity;
struct ScriptTimer *script_timers __asm__("D_8009F770");
s32 script_timer_count __asm__("D_8009F774");
void gInitScriptTimers(void) __asm__("func_80049400");
void gTickScriptTimers(s32 elapsed) __asm__("func_80049564");

/* VM boundary: 80069D68.c takes a script pointer; 80049564.c stores that
 * pointer as an s32. The fixture supplies separate true and false scripts.
 */
static u8 allowed_script, denied_script, action_script;
static struct Entity *vm_last_entity;
s32 gInterpretEntityScript(u8 *script, s32 arg1, s32 arg2, struct Entity *entity) __asm__("func_80069D68");
s32 gInterpretEntityScript(u8 *script, s32 arg1, s32 arg2, struct Entity *entity)
{
    vm_last_entity = entity;
    fck_record("script", (s32)script, arg1, arg2);
    return script == &allowed_script;
}

static void test_script_timers(void)
{
    struct ScriptTimer timers[6];

    /* @leaf 80049400 source src/game/boot-frontend/gInitScriptTimers.c: clear publication, not timer contents */
    LK_FUNC("func_80049400");
    lk_fill(timers, 0xA5, sizeof(timers));
    script_timers = timers;
    script_timer_count = 6;
    gInitScriptTimers();
    FCK_CHECK(script_timers == 0, "timer table unpublished");
    FCK_EQ(script_timer_count, 0, "timer count reset");
    FCK_CHECK(lk_all(timers, 0, sizeof(timers), 0xA5), "unpublished table not modified");

    /* @leaf 80049564 source src/game/boot-frontend/gTickScriptTimers.c: expiry, trigger gating and finite versus infinite repeats */
    LK_FUNC("func_80049564");
    timers[0].count = 0;
    timers[0].remaining = -1;
    timers[1].count = 2;
    timers[1].remaining = 4;
    timers[2].count = 2;
    timers[2].remaining = 3;
    timers[2].period = 11;
    timers[2].trigger = &allowed_script;
    timers[2].action = &action_script;
    timers[3] = timers[2];
    timers[3].count = -1;
    timers[3].remaining = -2;
    timers[4] = timers[2];
    timers[4].trigger = &denied_script;
    script_timers = timers;
    script_timer_count = 5;
    fck_log_reset();
    gTickScriptTimers(4);
    FCK_EQ(timers[0].remaining, -1, "disabled timer is not stepped");
    FCK_EQ(timers[1].remaining, 0, "exact zero is not yet expired");
    FCK_EQ(timers[2].remaining, 10, "finite timer adds period to negative remainder");
    FCK_EQ(timers[2].count, 1, "successful action uses one finite repeat");
    FCK_EQ(timers[3].remaining, 5, "infinite timer adds period to negative remainder");
    FCK_EQ(timers[3].count, -1, "negative repeat count never decremented");
    FCK_EQ(timers[4].remaining, 10, "denied trigger still reloads period");
    FCK_EQ(timers[4].count, 2, "denied trigger does not consume repeat");
    FCK_EQ(fck_log_count, 5, "one trigger per expiry and actions only on success");
    FCK_CHECK(fck_log[0].a0 == (s32)&allowed_script && fck_log[1].a0 == (s32)&action_script &&
              fck_log[2].a0 == (s32)&allowed_script && fck_log[3].a0 == (s32)&action_script &&
              fck_log[4].a0 == (s32)&denied_script, "trigger/action order across timers");
    FCK_CHECK(fck_log[0].a1 == 0 && fck_log[0].a2 == -1 && vm_last_entity == 0, "VM receives timer context");
    FCK_CHECK(lk_all(timers + 5, 0, sizeof(timers[0]), 0xA5), "backed record outside count untouched");
}

struct Unk4AADParent { u8 pad00[0x39], flags; u16 count; };
struct Unk4AADNode {
    u8 pad00[0x28], flags, pad29[0x1F];
    struct Unk4AADNode *next;
    struct Unk4AADParent *parent;
    u8 pad50[8];
};
struct Unk4AADOwner { u8 pad00[0x84]; struct Unk4AADNode *head; };
struct Unk4AADNode *fragment_free __asm__("D_8009F798");
struct Unk4AADNode *gFreeDebrisFragment(struct Unk4AADOwner *owner, struct Unk4AADNode *previous,
                                     struct Unk4AADNode *node) __asm__("func_8004AAD4");

static void test_fragments(void)
{
    struct Unk4AADOwner owner;
    struct Unk4AADNode nodes[4];
    struct Unk4AADParent parent;

    /* @leaf 8004AAD4 source src/game/boot-frontend/gFreeDebrisFragment.c: unlink active fragments, push free list and notify flagged parent */
    LK_FUNC("func_8004AAD4");
    lk_fill(&owner, 0x6D, sizeof(owner));
    lk_fill(nodes, 0xA5, sizeof(nodes));
    lk_fill(&parent, 0xA5, sizeof(parent));
    owner.head = nodes;
    nodes[0].next = nodes + 1;
    nodes[1].next = nodes + 2;
    nodes[2].next = nodes[3].next = 0;
    nodes[0].flags = 0x40;
    nodes[1].flags = 0x80;
    nodes[0].parent = nodes[1].parent = &parent;
    parent.flags = 0xA4;
    parent.count = 2;
    fragment_free = nodes + 3;
    FCK_CHECK(gFreeDebrisFragment(&owner, nodes, nodes + 1) == nodes, "middle unlink returns current active head");
    FCK_CHECK(nodes[0].next == nodes + 2, "predecessor bypasses freed fragment");
    FCK_CHECK(fragment_free == nodes + 1 && nodes[1].next == nodes + 3, "freed fragment prepended");
    FCK_EQ(parent.count, 1, "flagged fragment decrements parent count");
    FCK_EQ(parent.flags, 0xA5, "parent notification preserves other bits");
    FCK_CHECK(gFreeDebrisFragment(&owner, 0, nodes) == nodes + 2, "head unlink returns replacement");
    FCK_CHECK(owner.head == nodes + 2 && fragment_free == nodes && nodes[0].next == nodes + 1,
              "new active and free heads published");
    FCK_CHECK(parent.count == 1 && parent.flags == 0xA5, "unflagged fragment does not notify parent");
    FCK_CHECK(lk_all(&owner, 0, 0x84, 0x6D), "owner prefix untouched");
}

struct Unk69B1C {
    u8 pad00[0xE4];
    void *frame_time, *vm_aux, *screen_reset, *effect_id;
};
struct Unk77FFC { u8 pad00[0x1F], state, pad20[4]; };
u8 player_invulnerable[3] __asm__("D_8009FA8C");
void *terrain_heightfield[2] __asm__("D_800A294C");
u8 terrain_effect[2] __asm__("D_800A2931");
s32 terrain_queue_count __asm__("D_8009F838");
struct Unk69B1C *game_session __asm__("D_8009FA94");
u8 frame_time[4] __asm__("D_8009FAA4");
u8 vm_aux[4] __asm__("D_8009FAB4");
u8 screen_reset[4] __asm__("D_8009FA70");
u8 effect_id[4] __asm__("D_8009FACC");
struct Unk77FFC precincts[6] __asm__("D_8009B4E4");
s8 precinct_order[4] __asm__("D_8009B72C");
u8 precinct_unlocked __asm__("D_8009F3C2");
s32 deferred_transition __asm__("D_8009F8C8");
void *pending_capture __asm__("D_8009F8C4");
u8 rules_movie_name[4] __asm__("D_80012088");
void gResetPlayerInvulnFlags(void) __asm__("func_8005A044");
void gClearTerrainState(void) __asm__("func_80063134");
void gBindGameSessionPointers(void) __asm__("func_80069B1C");
void gUpdatePrecinctUnlockFlag(void) __asm__("func_80077FFC");
void gQueueCwRulesMovie(s32 selector) __asm__("func_8007BFAC");

static void test_game_state(void)
{
    struct Unk69B1C session;
    struct Unk77FFC snapshot[6];
    int i;

    /* @leaf 8005A044 source src/game/boot-frontend/gResetPlayerInvulnFlags.c: reset exactly two player flag bytes */
    LK_FUNC("func_8005A044");
    player_invulnerable[0] = 0x40;
    player_invulnerable[1] = 1;
    player_invulnerable[2] = 0xB7;
    gResetPlayerInvulnFlags();
    FCK_CHECK(player_invulnerable[0] == 0 && player_invulnerable[1] == 0, "both players reset");
    FCK_EQ(player_invulnerable[2], 0xB7, "backed byte outside two players untouched");

    /* @leaf 80063134 source src/game/gpu-2d-hud/gClearTerrainState.c: reset terrain publications, not adjacent slots */
    LK_FUNC("func_80063134");
    terrain_heightfield[0] = &session;
    terrain_heightfield[1] = snapshot;
    terrain_effect[0] = 0x87;
    terrain_effect[1] = 0x62;
    terrain_queue_count = -8;
    gClearTerrainState();
    FCK_CHECK(terrain_heightfield[0] == 0 && terrain_heightfield[1] == snapshot, "only first heightfield slot cleared");
    FCK_CHECK(terrain_effect[0] == 0 && terrain_effect[1] == 0x62, "only first effect byte cleared");
    FCK_EQ(terrain_queue_count, 0, "terrain queue count reset");

    /* @leaf 80069B1C source src/game/boot-frontend/gBindGameSessionPointers.c: bind four distinct addresses without writing pointed-to bytes */
    LK_FUNC("func_80069B1C");
    lk_fill(&session, 0xA5, sizeof(session));
    lk_fill(frame_time, 0x19, sizeof(frame_time));
    lk_fill(vm_aux, 0x2A, sizeof(vm_aux));
    lk_fill(screen_reset, 0x3B, sizeof(screen_reset));
    lk_fill(effect_id, 0x4C, sizeof(effect_id));
    game_session = &session;
    gBindGameSessionPointers();
    FCK_CHECK(session.frame_time == frame_time, "frame-time address bound");
    FCK_CHECK(session.vm_aux == vm_aux, "VM auxiliary address bound");
    FCK_CHECK(session.screen_reset == screen_reset, "screen-reset address bound");
    FCK_CHECK(session.effect_id == effect_id, "effect address bound");
    FCK_CHECK(lk_all(&session, 0, 0xE4, 0xA5), "session prefix untouched");
    FCK_CHECK(lk_all(frame_time, 0, 4, 0x19) && lk_all(vm_aux, 0, 4, 0x2A) &&
              lk_all(screen_reset, 0, 4, 0x3B) && lk_all(effect_id, 0, 4, 0x4C), "bound values not modified");

    /* @leaf 80077FFC source src/game/boot-frontend/gUpdatePrecinctUnlockFlag.c: only the four indexed completion states determine unlock */
    LK_FUNC("func_80077FFC");
    lk_fill(precincts, 0xA5, sizeof(precincts));
    precinct_order[0] = 5;
    precinct_order[1] = 2;
    precinct_order[2] = 0;
    precinct_order[3] = 4;
    for (i = 0; i < 4; i++) precincts[precinct_order[i]].state = 10;
    memcpy(snapshot, precincts, sizeof(snapshot));
    precinct_unlocked = 0xA5;
    gUpdatePrecinctUnlockFlag();
    FCK_EQ(precinct_unlocked, 1, "four completed selected precincts unlock");
    FCK_EQ(memcmp(snapshot, precincts, sizeof(snapshot)), 0, "unlock leaves precinct data unchanged");
    for (i = 0; i < 4; i++) {
        precincts[precinct_order[i]].state = 9;
        precinct_unlocked = 1;
        gUpdatePrecinctUnlockFlag();
        FCK_EQ(precinct_unlocked, 0, "each selected incomplete precinct prevents unlock");
        precincts[precinct_order[i]].state = 10;
    }
    for (i = 0; i < 4; i++) precincts[precinct_order[i]].state = 11;
    gUpdatePrecinctUnlockFlag();
    FCK_EQ(precinct_unlocked, 0, "unanimous but wrong state does not unlock");

    /* @leaf 8007BFAC source src/game/boot-frontend/gQueueCwRulesMovie.c: selector three publishes transition and filename */
    LK_FUNC("func_8007BFAC");
    deferred_transition = -5;
    pending_capture = &session;
    lk_fill(rules_movie_name, 0x6D, sizeof(rules_movie_name));
    gQueueCwRulesMovie(2);
    gQueueCwRulesMovie(4);
    FCK_CHECK(deferred_transition == -5 && pending_capture == &session, "neighbor selectors do not queue movie");
    gQueueCwRulesMovie(3);
    FCK_EQ(deferred_transition, 3, "rules movie transition published");
    FCK_CHECK(pending_capture == rules_movie_name, "rules movie name address published");
    FCK_CHECK(lk_all(rules_movie_name, 0, sizeof(rules_movie_name), 0x6D), "filename bytes not written");
}

struct MovieAudioFixture {
    volatile u32 read_cursor;
    u32 write_cursor;
    u8 buffer[0x7B18];
    s32 active;
    volatile s32 bytes, blocks;
    u8 pad7B2C[0x4C];
    s32 blocked;
};
struct MovieAudioFixture *movie_audio __asm__("D_8009F4D8");
void *gPeekMovieAudioSecondBuffer(u32 arg) __asm__("func_80086A4C");
void gAdvanceMovieAudioCounters(void) __asm__("func_80086D74");

static void test_movie_audio(void)
{
    static struct MovieAudioFixture state;
    static struct MovieAudioFixture snapshot;

    /* @leaf 80086A4C source src/game/boot-frontend/gPeekMovieAudioSecondBuffer.c: wrapped second-buffer pointer, exact capacity and busy gate */
    LK_FUNC("func_80086A4C");
    lk_fill(&state, 0xA5, sizeof(state));
    movie_audio = &state;
    state.read_cursor = 0x3C13;
    state.write_cursor = 0x3C23;
    state.blocked = 0;
    memcpy(&snapshot, &state, sizeof(state));
    FCK_CHECK(gPeekMovieAudioSecondBuffer(15) == (u8 *)&state + 0x3C23, "one byte below availability returns wrapped pointer");
    FCK_CHECK(gPeekMovieAudioSecondBuffer(16) == 0, "exact availability is rejected");
    FCK_CHECK(gPeekMovieAudioSecondBuffer(0xFFFFFFFFU) == 0, "oversized unsigned request rejected");
    FCK_EQ(memcmp(&state, &snapshot, sizeof(state)), 0, "peek neither consumes data nor modifies RAM");
    state.blocked = -1;
    FCK_CHECK(gPeekMovieAudioSecondBuffer(0) == 0, "busy state rejects request even with room");
    FCK_EQ(state.blocked, -1, "peek does not clear busy state");

    /* @leaf 80086D74 source src/game/boot-frontend/gAdvanceMovieAudioCounters.c: active-only increments of bytes and blocks */
    LK_FUNC("func_80086D74");
    state.active = 0;
    state.bytes = 0x3BF0;
    state.blocks = 5;
    memcpy(&snapshot, &state, sizeof(state));
    gAdvanceMovieAudioCounters();
    FCK_EQ(memcmp(&state, &snapshot, sizeof(state)), 0, "inactive call preserves all state");
    state.active = snapshot.active = -3;
    snapshot.bytes = 0x3DF0;
    snapshot.blocks = 7;
    gAdvanceMovieAudioCounters();
    gAdvanceMovieAudioCounters();
    FCK_EQ(state.bytes, 0x3DF0, "two active calls advance bytes without ring wrapping");
    FCK_EQ(state.blocks, 7, "two active calls add two blocks");
    FCK_EQ(memcmp(&state, &snapshot, sizeof(state)), 0, "only two counters changed");
}

/* 8008C07C.c: this layout is distinct from mode-select's port record. */
typedef struct {
    u8 pad00[0x28];
    u8 *data0, *data1;
    u8 pad30[4], count0, count1, mode, pad37[0xE], index, pad46[0x11];
    u8 enabled[6], guard[3];
} Object;
int gReadPadTxByte(Object *obj) __asm__("func_8008C07C");
s32 gReadAnalogStick(s32 *x, s32 *y, s32 port, s32 stick) __asm__("func_8001E7B8");
struct PadBuffer analog_pad_buffers[2] __asm__("D_800A78F0");

static void test_pad_reads(void)
{
    Object pad, snapshot;
    u8 primary[8] = { 0xC3, 0x82, 0x67, 0x65, 0xEF, 0x11, 0x9C, 0xCD };
    u8 secondary[4] = { 0x42, 0xFF, 0x81, 0xCC };
    struct PadBuffer analog_snapshot[2];
    s32 axes[4];

    /* @leaf 8008C07C source src/game/boot-frontend/gReadPadTxByte.c: mode-specific indexed reads, enables and out-of-range defaults */
    LK_FUNC("func_8008C07C");
    lk_fill(&pad, 0, sizeof(pad));
    pad.data0 = primary;
    pad.data1 = secondary;
    pad.count0 = 7;
    pad.count1 = 3;
    pad.enabled[0] = 1;
    pad.index = 3;
    FCK_EQ(gReadPadTxByte(&pad), 0xC3, "base index reads first primary byte unsigned");
    pad.index = 4;
    FCK_EQ(gReadPadTxByte(&pad), 0, "disabled primary byte masked");
    pad.index = 9;
    FCK_EQ(gReadPadTxByte(&pad), 0x9C, "index six no longer uses enables table");
    pad.index = 10;
    FCK_EQ(gReadPadTxByte(&pad), 0, "primary byte at count boundary rejected");
    pad.mode = 0x4D;
    pad.index = 5;
    FCK_EQ(gReadPadTxByte(&pad), 0x81, "mode 4D reads last available secondary byte");
    pad.index = 6;
    FCK_EQ(gReadPadTxByte(&pad), 0xFF, "mode 4D fills missing byte with FF");
    pad.mode = 0x12;
    FCK_EQ(gReadPadTxByte(&pad), 0, "other mode fills missing byte with zero");
    pad.index = 3;
    memcpy(&snapshot, &pad, sizeof(snapshot));
    FCK_EQ(gReadPadTxByte(&pad), 0x42, "other mode selects secondary buffer");
    FCK_EQ(memcmp(&pad, &snapshot, sizeof(pad)), 0, "read does not consume index or alter port");
    FCK_CHECK(primary[0] == 0xC3 && secondary[0] == 0x42, "input bytes untouched");

    /* @leaf 8001E7B8 source src/game/platform/gReadAnalogStick.c: pad type gates axis writes; only selector one chooses right stick */
    LK_FUNC("func_8001E7B8");
    lk_fill(analog_pad_buffers, 0x6D, sizeof(analog_pad_buffers));
    analog_pad_buffers[0].unk00[0] = 0;
    analog_pad_buffers[0].unk00[1] = 0x41;
    analog_pad_buffers[1].unk00[0] = 0;
    analog_pad_buffers[1].unk00[1] = 0x73;
    analog_pad_buffers[1].rightX = 0xFF;
    analog_pad_buffers[1].rightY = 0x04;
    analog_pad_buffers[1].leftX = 0x2C;
    analog_pad_buffers[1].leftY = 0xA9;
    memcpy(analog_snapshot, analog_pad_buffers, sizeof(analog_snapshot));
    axes[0] = 11; axes[1] = -33; axes[2] = -44; axes[3] = 22;
    FCK_EQ(gReadAnalogStick(axes + 1, axes + 2, 0, 1), 0, "digital controller rejected");
    FCK_CHECK(axes[1] == -33 && axes[2] == -44, "rejected controller leaves outputs alone");
    FCK_EQ(gReadAnalogStick(axes + 1, axes + 2, 1, 1), 1, "right stick available on second port");
    FCK_CHECK(axes[1] == 255 && axes[2] == 4, "right axes widened without sign extension");
    FCK_EQ(gReadAnalogStick(axes + 1, axes + 2, 1, -1), 1, "non-one selector uses left stick");
    FCK_CHECK(axes[1] == 44 && axes[2] == 169, "left axes written");
    FCK_CHECK(axes[0] == 11 && axes[3] == 22, "output guards unchanged");
    FCK_EQ(memcmp(analog_snapshot, analog_pad_buffers, sizeof(analog_snapshot)), 0, "pad buffers untouched");
}

void gSetFlatTrianglePacketHeader(unsigned char *p) __asm__("func_80092460");
void gSetSprt8(u8 *arg0) __asm__("func_80092500");
typedef struct { u8 iq_y[64], iq_c[64]; s16 dct[64]; } DecDctEnv;
s32 mdec_y[16] __asm__("D_8009E8B4");
s32 mdec_c[16] __asm__("D_8009E8F4");
s32 mdec_dct[32] __asm__("D_8009E938");
DecDctEnv *gInitDefaultMdecEnvironment(DecDctEnv *environment) __asm__("func_80095EC4");

static void test_packet_headers(void)
{
    u8 packet[16];
    u8 triangle[16] = { 0xA5, 0xA5, 0xA5, 0xA5, 4, 0xA5, 0xA5, 0xA5,
                        0x20, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5 };
    u8 sprite[16] = { 0x6D, 0x6D, 0x6D, 0x6D, 0x6D, 3, 0x6D, 0x6D,
                      0x6D, 0x74, 0x6D, 0x6D, 0x6D, 0x6D, 0x6D, 0x6D };

    /* @leaf 80092460 source src/game/other/gSetFlatTrianglePacketHeader.c: flat triangle length and code bytes only */
    LK_FUNC("func_80092460");
    lk_fill(packet, 0xA5, sizeof(packet));
    gSetFlatTrianglePacketHeader(packet + 1);
    FCK_EQ(packet[4], 4, "triangle command word count");
    FCK_EQ(packet[8], 0x20, "flat triangle opcode");
    FCK_EQ(memcmp(packet, triangle, sizeof(triangle)), 0, "all other packet and guard bytes remain");

    /* @leaf 80092500 source src/game/gpu-2d-hud/gSetSprt8.c: 8x8 sprite length and code bytes only */
    LK_FUNC("func_80092500");
    lk_fill(packet, 0x6D, sizeof(packet));
    gSetSprt8(packet + 2);
    FCK_EQ(packet[5], 3, "sprite command word count");
    FCK_EQ(packet[9], 0x74, "eight-pixel sprite opcode");
    FCK_EQ(memcmp(packet, sprite, sizeof(sprite)), 0, "all other packet and guard bytes remain");
}

static void test_mdec_defaults(void)
{
    struct { u32 before; DecDctEnv env; u32 after; } fixture;
    int i;

    /* @leaf 80095EC4 source src/game/gpu-2d-hud/gInitDefaultMdecEnvironment.c: three full table copies with stable destination identity */
    LK_FUNC("func_80095EC4");
    lk_fill(&fixture, 0xA5, sizeof(fixture));
    for (i = 0; i < 16; i++) {
        mdec_y[i] = 0x10203040 + i;
        mdec_c[i] = 0x50607080 - i;
    }
    for (i = 0; i < 32; i++) mdec_dct[i] = 0x76A8E013 + 0x10000 * i;
    FCK_CHECK(gInitDefaultMdecEnvironment(&fixture.env) == &fixture.env, "returns caller's environment pointer");
    FCK_EQ(memcmp(fixture.env.iq_y, mdec_y, sizeof(mdec_y)), 0, "complete luminance table copied");
    FCK_EQ(memcmp(fixture.env.iq_c, mdec_c, sizeof(mdec_c)), 0, "complete chrominance table copied");
    FCK_EQ(memcmp(fixture.env.dct, mdec_dct, sizeof(mdec_dct)), 0, "complete DCT table copied");
    FCK_CHECK(fixture.before == 0xA5A5A5A5U && fixture.after == 0xA5A5A5A5U, "environment guards untouched");
    FCK_CHECK(mdec_y[15] == 0x1020304F && mdec_c[15] == 0x50607071 && mdec_dct[31] == 0x76C7E013,
              "source tails unchanged");
}

struct Unk3CA80 {
    u8 pad00[4];
    s16 identifier;
    u8 kind;
    s8 slot;
    u8 pad08[7], flags, pad10[4];
    s16 counter;
    u8 pad16[0x0E];
    void (*callback)(s32 message, struct Unk3CA80 *object, s32 arg2, s32 arg3);
};
void gRetireSpawnSlot(struct Unk3CA80 *object) __asm__("func_8003CA80");
static int retirement_called_before_reset;

static void record_slot_retirement(s32 message, struct Unk3CA80 *object, s32 arg2, s32 arg3)
{
    fck_record("retire", message, (s32)object, arg2);
    retirement_called_before_reset = object->identifier == 800 && object->slot == -5 &&
        object->kind == 0xF1 && object->counter == 13 && object->flags == 0xA5 && arg3 == 0;
}

static void test_spawn_retirement(void)
{
    struct Unk3CA80 slot, expected;

    /* @leaf 8003CA80 source src/game/gameplay-ai/gRetireSpawnSlot.c: teardown callback sees live slot before reset */
    LK_FUNC("func_8003CA80");
    lk_fill(&slot, 0xA5, sizeof(slot));
    slot.identifier = 800;
    slot.slot = -5;
    slot.kind = 0xF1;
    slot.counter = 13;
    slot.callback = record_slot_retirement;
    memcpy(&expected, &slot, sizeof(slot));
    expected.identifier = expected.slot = -1;
    expected.counter = expected.kind = 0;
    expected.flags = 0xA7;
    fck_log_reset();
    retirement_called_before_reset = 0;
    gRetireSpawnSlot(&slot);
    FCK_EQ(fck_log_count, 1, "one teardown callback");
    FCK_CHECK(fck_log[0].a0 == 7 && fck_log[0].a1 == (s32)&slot && fck_log[0].a2 == 0,
              "teardown message, slot and first zero argument forwarded");
    FCK_CHECK(retirement_called_before_reset, "callback observes unmodified slot and second zero argument");
    FCK_EQ(memcmp(&slot, &expected, sizeof(slot)), 0, "slot fields reset without touching other fields");
}

struct FontEntry {
    u8 pad00[6];
    s16 height;
    u8 pad08[8];
    u8 *values;
    u8 pad14[8], index[256];
};
struct FontEntry *active_font __asm__("D_8009F35C");
void gMeasureTextExtent(u8 *text, s32 *width, s32 *height) __asm__("func_800492A0");
s32 gMeasureTextWidthPx(u8 *text) __asm__("func_80049340");

static void test_text_metrics(void)
{
    struct FontEntry font, snapshot;
    u8 glyphs[3][11], glyph_snapshot[3][11];
    u8 line[] = { 0x1F, 0x20, 'A', 0xE3, 'A', '\0', 'A' };
    u8 controls[] = { '\n', 1, 0x1F, '\0' };
    s32 out[4];

    lk_fill(&font, 0xA5, sizeof(font));
    lk_fill(glyphs, 0x6D, sizeof(glyphs));
    font.height = 21;
    font.values = glyphs[0];
    font.index[0x1F] = 2; /* must be skipped despite a valid glyph */
    font.index[0x20] = 1;
    font.index['A'] = 0;
    font.index[0xE3] = 2;
    glyphs[0][8] = 7;
    glyphs[1][8] = 3;
    glyphs[2][8] = 0xFC; /* signed advance -4 */
    memcpy(&snapshot, &font, sizeof(font));
    memcpy(glyph_snapshot, glyphs, sizeof(glyphs));

    /* @leaf 800492A0 source src/game/gpu-2d-hud/gMeasureTextExtent.c: signed advance accumulation and height output */
    LK_FUNC("func_800492A0");
    active_font = &font;
    out[0] = 123; out[1] = -7; out[2] = -9; out[3] = 456;
    gMeasureTextExtent(line, out + 1, out + 2);
    FCK_EQ(out[1], 13, "extent sums space, signed high-byte glyph and two As");
    FCK_EQ(out[2], 21, "extent publishes font height");
    gMeasureTextExtent(controls, out + 1, out + 2);
    FCK_CHECK(out[1] == 0 && out[2] == 21, "controls produce no advance but retain line height");
    active_font = 0;
    gMeasureTextExtent(line, out + 1, out + 2);
    FCK_CHECK(out[1] == 0 && out[2] == 0, "missing font zeroes both outputs");
    FCK_CHECK(out[0] == 123 && out[3] == 456, "only the two output words changed");
    FCK_CHECK(memcmp(&font, &snapshot, sizeof(font)) == 0 &&
              memcmp(glyphs, glyph_snapshot, sizeof(glyphs)) == 0, "extent leaves font data untouched");

    /* @leaf 80049340 source src/game/gpu-2d-hud/gMeasureTextWidthPx.c: doubled width with no modification of the font or text */
    LK_FUNC("func_80049340");
    active_font = &font;
    FCK_EQ(gMeasureTextWidthPx(line), 26, "pixel width doubles signed advances");
    FCK_EQ(gMeasureTextWidthPx(controls), 0, "controls contribute no width");
    FCK_EQ(gMeasureTextWidthPx(line + 3) - gMeasureTextWidthPx(line + 4), -8, "high-byte glyph subtracts eight pixels");
    FCK_CHECK(memcmp(&font, &snapshot, sizeof(font)) == 0 &&
              memcmp(glyphs, glyph_snapshot, sizeof(glyphs)) == 0, "width leaves font data untouched");
    FCK_CHECK(line[0] == 0x1F && line[3] == 0xE3 && line[5] == 0 && line[6] == 'A',
              "width preserves control, signed-advance glyph and terminator bytes");
    active_font = 0;
    FCK_EQ(gMeasureTextWidthPx(line), 0, "missing font reports zero width");
}

struct Entry { u8 data[0x68]; u32 sum, other; };
struct Save {
    u8 pad[0x40C], header[0x10];
    u32 sum, other;
    struct Entry entries[10];
    u8 pad2[0x923 - 0x424 - 10 * 0x70], valid;
};
struct Save *save_image __asm__("D_8009F92C");
void gUpdateProfileChecksums(void) __asm__("func_8007F6E0");
s32 gVerifySaveImageChecksums(void) __asm__("func_8007F750");
s32 gVerifyDualChecksum(u8 *data, u32 len, u32 expectedSum, u32 expectedOther, s32 clear)
    __asm__("func_8007F824");

/* For length 104 the zero indices contribute 5356. These shift-XOR
 * results apply when only the last byte is nonzero (from 1 to 10).
 */
static const u32 entry_other[10] = {
    0xFFFFFF99U, 0xFFFFFF9BU, 0xFFFFFF9DU, 0xFFFFFF9FU, 0xFFFFFFA1U,
    0xFFFFFFA3U, 0xFFFFFFA5U, 0xFFFFFF87U, 0xFFFFFF89U, 0xFFFFFF8BU
};

static void prepare_save_payloads(struct Save *save)
{
    int i;
    lk_fill(save, 0xA5, sizeof(*save));
    lk_fill(save->header, 0, sizeof(save->header));
    save->header[15] = 37;
    for (i = 0; i < 10; i++) {
        lk_fill(save->entries[i].data, 0, sizeof(save->entries[i].data));
        save->entries[i].data[0x67] = i + 1;
    }
}

static void test_profile_checksums(void)
{
    struct Save save, expected;
    u8 region[7] = { 0xA5, 0, 0xFF, 19, 0x80, 31, 0x6D };
    u8 original[7];
    int i;

    /* @leaf 8007F6E0 source src/game/memory-resources/gUpdateProfileChecksums.c: update exactly 11 checksum pairs without editing payloads */
    LK_FUNC("func_8007F6E0");
    prepare_save_payloads(&save);
    memcpy(&expected, &save, sizeof(save));
    expected.sum = 157;
    expected.other = 0xFFB9;
    for (i = 0; i < 10; i++) {
        expected.entries[i].sum = 5357 + i;
        expected.entries[i].other = entry_other[i];
    }
    save_image = &save;
    gUpdateProfileChecksums();
    FCK_EQ(save.sum, 157, "options checksum sum");
    FCK_EQ(save.other, 0xFFB9, "options shift-XOR checksum");
    for (i = 0; i < 10; i++) {
        FCK_EQ(save.entries[i].sum, 5357 + i, "entry checksum sum");
        FCK_EQ(save.entries[i].other, entry_other[i], "entry shift-XOR checksum");
    }
    FCK_EQ(memcmp(&save, &expected, sizeof(save)), 0, "only the checksum words changed");

    /* @leaf 8007F750 source src/game/platform/gVerifySaveImageChecksums.c: header validity distinct from aggregate status and failed-entry clearing */
    LK_FUNC("func_8007F750");
    memcpy(&save, &expected, sizeof(save)); /* load expected checksum words, not output from updater */
    expected.valid = 1;
    FCK_EQ(gVerifySaveImageChecksums(), 1, "valid header and all ten entries accepted");
    FCK_EQ(memcmp(&save, &expected, sizeof(save)), 0, "valid image changes only validity byte");

    save.entries[0].data[0] = 51;
    save.entries[9].data[8] = 99;
    lk_fill(expected.entries[0].data, 0, sizeof(expected.entries[0].data));
    lk_fill(expected.entries[9].data, 0, sizeof(expected.entries[9].data));
    FCK_EQ(gVerifySaveImageChecksums(), 0, "bad entries reject image");
    FCK_EQ(save.valid, 1, "validity byte tracks header, not entries");
    FCK_EQ(memcmp(&save, &expected, sizeof(save)), 0, "first and last bad entry cleared, all other bytes preserved");

    memcpy(&save, &expected, sizeof(save));
    save.header[0] = 0x6D;
    save.entries[8].data[8] = 99;
    expected.header[0] = 0x6D;
    expected.valid = 0;
    lk_fill(expected.entries[8].data, 0, sizeof(expected.entries[8].data));
    FCK_EQ(gVerifySaveImageChecksums(), 0, "bad header rejects image");
    FCK_EQ(save.valid, 0, "bad header clears validity");
    FCK_EQ(memcmp(&save, &expected, sizeof(save)), 0, "bad header retained but entry scanning still continues");

    /* @leaf 8007F824 source src/game/utilities/gVerifyDualChecksum.c: independent checksum fields and exact optional clear region */
    LK_FUNC("func_8007F824");
    memcpy(original, region, sizeof(region));
    FCK_EQ(gVerifyDualChecksum(region + 1, 5, 443, 0xD54, -1), 1, "matching pair succeeds even with clear enabled");
    FCK_EQ(memcmp(region, original, sizeof(region)), 0, "success does not clear input");
    FCK_EQ(gVerifyDualChecksum(region + 1, 5, 444, 0xD54, 0), 0, "bad additive checksum rejected separately");
    FCK_EQ(memcmp(region, original, sizeof(region)), 0, "clear disabled retains input on mismatch");
    FCK_EQ(gVerifyDualChecksum(region + 1, 5, 443, 0xD55, -1), 0, "bad shift-XOR checksum rejected separately");
    FCK_CHECK(lk_all(region, 1, 6, 0) && region[0] == 0xA5 && region[6] == 0x6D,
              "clear touches precisely the requested five bytes");
    FCK_EQ(gVerifyDualChecksum(region + 1, 0, 0, 0, 1), 1, "empty input matches zero checksums");
}

struct PadPort {
    u8 pad00[0x46], sequence_step, pad47[0xC], mode_already_selected;
};
void (*pad_reset_hook)(struct PadPort *) __asm__("D_8009C72C");
s32 gRespPadModeSelect(struct PadPort *port) __asm__("func_8008B6BC");

static void record_pad_reset(struct PadPort *port)
{
    fck_record("reset port", (s32)port, port->sequence_step, port->mode_already_selected);
    port->sequence_step = 1;
}

static void test_pad_mode(void)
{
    struct PadPort port;

    /* @leaf 8008B6BC source src/game/boot-frontend/gRespPadModeSelect.c: step/status select return, rejection write or callback */
    LK_FUNC("func_8008B6BC");
    lk_fill(&port, 0xA5, sizeof(port));
    port.mode_already_selected = 0x90;
    port.sequence_step = 2;
    pad_reset_hook = record_pad_reset;
    fck_log_reset();
    FCK_EQ(gRespPadModeSelect(&port), 1, "selected mode at step two completes");
    FCK_EQ(port.sequence_step, 2, "completion leaves step untouched");
    port.sequence_step = 3;
    FCK_EQ(gRespPadModeSelect(&port), 0, "selected mode at another step fails");
    FCK_EQ(port.sequence_step, 0xFE, "selection failure writes terminal step");
    FCK_EQ(fck_log_count, 0, "selected paths never invoke reset callback");

    port.mode_already_selected = 0;
    port.sequence_step = 7;
    FCK_EQ(gRespPadModeSelect(&port), 0, "pending selection reports incomplete");
    FCK_EQ(fck_log_count, 1, "pending selection invokes reset once");
    FCK_CHECK(fck_log[0].a0 == (s32)&port && fck_log[0].a1 == 7 && fck_log[0].a2 == 0,
              "reset receives the original pending state");
    FCK_EQ(port.sequence_step, 1, "callback change persists");
    FCK_EQ(port.mode_already_selected, 0, "call does not select a pending mode");
    FCK_CHECK(lk_all(&port, 0, 0x46, 0xA5) && lk_all(&port, 0x47, 0x53, 0xA5),
              "other port bytes unchanged");
}

typedef struct { u16 *header; u8 pad04[0x18]; s32 version; } Unk800899A4;
typedef struct { u8 pad00[0x1C]; s32 index; } Unk8008263C;
Unk800899A4 *text_table __asm__("D_8009F518");
Unk8008263C **published_blocks __asm__("D_8009F3F4");
void gLoadTextLookupTable(Unk800899A4 *arg0) __asm__("func_800899A4");
void gRegisterPublishedBlock(Unk8008263C *arg) __asm__("func_8008263C");

/* Heap boundary: the actual 800181D4.c accepts void * and returns nothing.
 * These are stack fixtures, so record each release and the pre-release
 * publication state instead of running the allocator on them.
 */
void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeBlock(void *block)
{
    fck_record("free block", (s32)block, (s32)text_table, 0);
}

static void test_resource_publication(void)
{
    u16 good_header = 1, bad_header = 2;
    Unk800899A4 initial, accepted, rejected, bad_content;
    Unk8008263C *slots[31], resource[4];
    Unk8008263C *expected[31];

    /* @leaf 800899A4 source src/game/memory-resources/gLoadTextLookupTable.c: validate wrapper and header, freeing in publication order */
    LK_FUNC("func_800899A4");
    lk_fill(&initial, 0xA5, sizeof(initial));
    initial.header = &good_header;
    initial.version = 1;
    accepted = initial;
    rejected = initial;
    bad_content = initial;
    rejected.version = 2;
    bad_content.header = &bad_header;
    text_table = &initial;
    fck_log_reset();
    gLoadTextLookupTable(&rejected);
    FCK_CHECK(text_table == &initial, "bad wrapper leaves prior publication");
    FCK_EQ(fck_log_count, 1, "bad wrapper releases only caller's candidate");
    FCK_CHECK(fck_log[0].a0 == (s32)&rejected && fck_log[0].a1 == (s32)&initial,
              "bad wrapper freed while previous table remains active");
    gLoadTextLookupTable(&accepted);
    FCK_CHECK(text_table == &accepted, "valid candidate published");
    FCK_EQ(fck_log_count, 2, "replacement frees exactly the previous table");
    FCK_CHECK(fck_log[1].a0 == (s32)&initial && fck_log[1].a1 == (s32)&initial,
              "previous table freed before replacement publication");
    gLoadTextLookupTable(&bad_content);
    FCK_CHECK(text_table == 0, "bad inner header clears publication");
    FCK_EQ(fck_log_count, 4, "bad header releases old and new table");
    FCK_CHECK(fck_log[2].a0 == (s32)&accepted && fck_log[2].a1 == (s32)&accepted &&
              fck_log[3].a0 == (s32)&bad_content && fck_log[3].a1 == (s32)&bad_content,
              "old table released before the provisional bad table");
    gLoadTextLookupTable(&accepted);
    FCK_CHECK(text_table == &accepted && fck_log_count == 4, "valid table on empty slot needs no release");
    FCK_CHECK(good_header == 1 && bad_header == 2, "input headers not edited");

    /* @leaf 8008263C source src/game/boot-frontend/gRegisterPublishedBlock.c: bounded publication and release of collisions or invalid IDs */
    LK_FUNC("func_8008263C");
    lk_fill(slots, 0, sizeof(slots));
    lk_fill(resource, 0xA5, sizeof(resource));
    resource[0].index = 29;
    resource[1].index = 29;
    resource[2].index = 30;
    resource[3].index = 0;
    published_blocks = 0;
    fck_log_reset();
    gRegisterPublishedBlock(resource);
    FCK_EQ(fck_log_count, 0, "valid id with unpublished directory is ignored without freeing");
    published_blocks = slots;
    gRegisterPublishedBlock(resource);
    FCK_CHECK(slots[29] == resource && fck_log_count == 0, "last valid index published without release");
    gRegisterPublishedBlock(resource + 1);
    gRegisterPublishedBlock(resource + 2);
    gRegisterPublishedBlock(resource + 3);
    FCK_EQ(fck_log_count, 3, "duplicate, upper and lower invalid IDs each released");
    FCK_CHECK(fck_log[0].a0 == (s32)(resource + 1) && fck_log[1].a0 == (s32)(resource + 2) &&
              fck_log[2].a0 == (s32)(resource + 3), "releases match rejected resources in order");
    lk_fill(expected, 0, sizeof(expected));
    expected[29] = resource;
    FCK_EQ(memcmp(slots, expected, sizeof(slots)), 0, "directory retains original entry and guards only");
}

struct ModuleQueues { u8 pad00[0x10]; Node *recycled, *parsed, *retained; };
struct ModuleQueues module_queues __asm__("D_800A0B70");
void gRetireParsedModuleBuffer(void) __asm__("func_80028D28");

struct Unk5BD20Target { u8 pad00[8]; s32 id; };
struct Unk5BD20Node {
    struct Unk5BD20Node *next;
    struct Unk5BD20Target *first, *second;
    s32 unk0C, first_id, second_id;
    u8 pad18[0x12], flags;
};
struct Unk5BD20Pool { void *storage; struct Unk5BD20Node *active, *free; };
void gSweepEffectPool(struct Unk5BD20Pool *pool, void *owner) __asm__("func_8005BD20");

static void test_queue_retirement(void)
{
    Node buffers[4];
    struct Unk5BD20Pool pool;
    struct Unk5BD20Node effects[5];
    struct Unk5BD20Target targets[2];
    int i;

    /* @leaf 80028D28 source src/game/boot-frontend/gRetireParsedModuleBuffer.c: pop parsed head, append retained buffers, recycle unreferenced */
    LK_FUNC("func_80028D28");
    lk_fill(&module_queues, 0xA5, sizeof(module_queues));
    lk_fill(buffers, 0x6D, sizeof(buffers));
    for (i = 0; i < 3; i++) buffers[i].next = buffers + i + 1;
    buffers[3].next = 0;
    buffers[0].count = 7;
    buffers[1].count = 1;
    buffers[2].count = 0;
    module_queues.parsed = buffers;
    module_queues.recycled = module_queues.retained = 0;
    gRetireParsedModuleBuffer();
    FCK_CHECK(module_queues.parsed == buffers + 1 && module_queues.retained == buffers && buffers[0].next == 0,
              "first referenced record populates retained list while parsed advances");
    FCK_CHECK(buffers[0].count == 7 && buffers[0].unk08 == 0, "retention keeps refs but clears parsed status");
    gRetireParsedModuleBuffer();
    FCK_CHECK(buffers[0].next == buffers + 1 && buffers[1].next == 0 && module_queues.parsed == buffers + 2,
              "second referenced record appended");
    gRetireParsedModuleBuffer();
    FCK_CHECK(module_queues.recycled == buffers + 2 && module_queues.parsed == buffers + 3 && buffers[2].next == 0,
              "zero-ref record moved to free list and detached from parsed successor");
    FCK_CHECK(buffers[2].unk08 == 0 && buffers[2].unk04 == 0x6D6D6D6D, "retirement edits status but not adjacent word");
    FCK_CHECK(buffers[1].next == 0 && buffers[3].unk08 == 0x6D6D6D6D, "other retained and parsed records intact");
    FCK_CHECK(lk_all(&module_queues, 0, 0x10, 0xA5), "unrelated queue state left alone");

    /* @leaf 8005BD20 source src/game/renderer-scene/gSweepEffectPool.c: sweep flagged nodes and invalidate stale references on survivors */
    LK_FUNC("func_8005BD20");
    lk_fill(effects, 0, sizeof(effects));
    lk_fill(targets, 0x6D, sizeof(targets));
    targets[0].id = 101;
    targets[1].id = -30;
    for (i = 0; i < 3; i++) effects[i].next = effects + i + 1;
    effects[0].flags = 1;
    effects[1].flags = 0x80;
    effects[1].first = targets;
    effects[1].first_id = 100;
    effects[1].second = targets + 1;
    effects[1].second_id = -30;
    effects[2].flags = 0x81;
    effects[3].first = targets;
    effects[3].first_id = 101;
    effects[3].second = targets + 1;
    effects[3].second_id = -31;
    pool.storage = effects;
    pool.active = effects;
    pool.free = effects + 4;
    gSweepEffectPool(&pool, targets);
    FCK_CHECK(pool.active == effects + 1 && effects[1].next == effects + 3 && effects[3].next == 0,
              "head and middle flagged nodes removed from active list");
    FCK_CHECK(pool.free == effects + 2 && effects[2].next == effects && effects[0].next == effects + 4 &&
              effects[4].next == 0, "removed nodes prepended in traversal order");
    FCK_CHECK(effects[1].first == 0 && effects[1].second == targets + 1, "stale first reference cleared only");
    FCK_CHECK(effects[3].first == targets && effects[3].second == 0, "stale second reference cleared only");
    FCK_CHECK(pool.storage == effects && effects[1].flags == 0x80 && effects[2].flags == 0x81,
              "pool storage and non-link flags preserved");
    FCK_CHECK(targets[0].id == 101 && targets[1].id == -30, "target records untouched");
}

int main(void)
{
    test_dma_queue();
    test_motion();
    test_sound_lists();
    test_script_timers();
    test_fragments();
    test_game_state();
    test_movie_audio();
    test_pad_reads();
    test_packet_headers();
    test_mdec_defaults();
    test_spawn_retirement();
    test_text_metrics();
    test_profile_checksums();
    test_pad_mode();
    test_resource_publication();
    test_queue_retirement();
    return lk_done("leaf_thirty");
}
