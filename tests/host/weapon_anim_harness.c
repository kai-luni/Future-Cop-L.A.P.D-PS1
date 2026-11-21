/* Behavioural checks for the weapon slot driver (func_8006BA50).
 *
 * The unit itself is compiled unchanged and linked next to this harness, so the
 * checks below run the real decomp C: the reload gate, the empty-counter sound,
 * the counter refill/toggle, the cycle counter reload and the alternating
 * muzzle swap. Everything the unit calls out to is provided here as a stand-in
 * with its retail label, so a broken binding fails at link time.
 *
 * The offsets and the rules asserted here are the ones the unit's doc block
 * states, which makes this a regression guard for both the code and the doc. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

void __main(void) {}

static int failures;

static void say(const char *text)
{
    int length = 0;
    register long v0 __asm__("$2") = 4004;
    register long a0 __asm__("$4") = 1;
    register long a1 __asm__("$5") = (long)text;
    register long a2 __asm__("$6");
    while (text[length]) length++;
    a2 = length;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
}

#define CHECK(condition, message) do { \
    if (!(condition)) { say("FAIL: " message "\n"); failures++; } \
} while (0)

/* --- the control block layout the unit expects ---------------------------- */
struct WNode {
    struct WNode *next;
    u8 pad04[4];
    s32 unk08;
    u8 pad0C[8];
    u16 unk14;
    u8 unk16;
    u8 pad17[5];
    s16 unk1C;
    u8 pad1E[0xA6];
    s32 unkC4;
};

struct WCtl {
    u8 pad00[0x78];
    s32 reload[3];
    s16 perSlot[3];
    s16 secondary[3];
    u8 id[3];
    u8 cycle[3];
    u8 pad96[2];
    struct WNode *pending;
    s32 pendingId;
    u8 padA0[0x21];
    s8 ownerSlot;
};

struct WObj {
    u8 pad00[0xAC];
    struct WCtl *ctl;
};

struct WEntry {
    u8 type;
    u8 pad01[3];
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    s16 unk08;
    s16 unk0A;
    u8 pad0C[0x13];
    u8 unk1F;
};

/* The record gRegistryFind returns: two parameter blocks at +0x04 and +0x24 and
 * the alternating muzzle halves at +0x44 / +0x50. */
struct WTgt {
    u8 pad00[4];
    struct WEntry normal;
    u8 pad24[0x24 - 4 - sizeof(struct WEntry)];
    struct WEntry flagged;
    s32 unk44;
    u8 pad48[8];
    s32 unk50;
};

/* --- the unit under test and everything it calls -------------------------- */
void gWeaponSlotUpdate(struct WObj *obj, s32 idx) __asm__("func_8006BA50");

struct WNode *gUnitListHead __asm__("D_8009FA40");
u8 gPlayerInvulnerable[8] __asm__("D_8009FA8C");

static s32 sound_log[8];
static s32 sound_count;
static s32 fire_calls;
static s32 fire_result;
static s32 beam_calls;
static s32 beam_last_type;
static struct WTgt registry_record;

s32 gPlayNonpositionalSoundEffect(s32 id, struct WObj *obj) __asm__("func_80046C84");
s32 gPlayNonpositionalSoundEffect(s32 id, struct WObj *obj)
{
    (void)obj;
    if (sound_count < 8) sound_log[sound_count++] = id;
    return 0;
}

struct WTgt *gRegistryFind(u8 id) __asm__("func_8006AAEC");
struct WTgt *gRegistryFind(u8 id)
{
    (void)id;
    return &registry_record;
}

s32 gWeaponFireDispatch(struct WObj *obj, s32 idx) __asm__("func_8006ACB0");
s32 gWeaponFireDispatch(struct WObj *obj, s32 idx)
{
    (void)obj;
    (void)idx;
    fire_calls++;
    return fire_result;
}

void gBeamPartUpdate(struct WObj *obj, s32 idx, s32 arg2) __asm__("func_8006B23C");
void gBeamPartUpdate(struct WObj *obj, s32 idx, s32 arg2)
{
    (void)obj;
    (void)idx;
    beam_calls++;
    beam_last_type = arg2;
}

/* --- fixtures ------------------------------------------------------------- */
static struct WCtl ctl;
static struct WObj obj;

static void reset(void)
{
    u8 *p = (u8 *)&ctl;
    s32 i;

    for (i = 0; i < (s32)sizeof(ctl); i++) p[i] = 0;
    p = (u8 *)&registry_record;
    for (i = 0; i < (s32)sizeof(registry_record); i++) p[i] = 0;
    gUnitListHead = 0;
    gPlayerInvulnerable[0] = 0;
    obj.ctl = &ctl;
    sound_count = 0;
    fire_calls = 0;
    fire_result = 1;
    beam_calls = 0;
    beam_last_type = 0;
    ctl.ownerSlot = 0;
    ctl.perSlot[0] = 3;
    registry_record.normal.type = 1;
    registry_record.normal.unk06 = 9;
    registry_record.normal.unk08 = 4;
    registry_record.normal.unk0A = 7;
    registry_record.flagged.type = 1;
    registry_record.flagged.unk06 = 9;
    registry_record.flagged.unk08 = 4;
    registry_record.flagged.unk0A = 7;
    ctl.cycle[0] = 2;
    ctl.reload[0] = 0;
}

/* --- 1. the slot index gate ---------------------------------------------- */
static void test_index_gate(void)
{
    reset();
    /* Leave every in-range field neutral: if the gate ever widened, slot 3
     * would walk past the per-slot arrays and play the empty-clip sound. */
    ctl.perSlot[0] = 0;
    ctl.secondary[0] = 0;
    ctl.cycle[0] = 2;
    gWeaponSlotUpdate(&obj, 3);
    CHECK(ctl.cycle[0] == 2, "an out-of-range slot does nothing");
    CHECK(sound_count == 0, "an out-of-range slot plays no sound");
    CHECK(fire_calls == 0 && beam_calls == 0, "an out-of-range slot dispatches nothing");
}

/* --- 2. the reload timer gate -------------------------------------------- */
static void test_reload_gate(void)
{
    reset();
    ctl.reload[0] = 1;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(fire_calls == 0, "a running reload timer suppresses the shot");
    CHECK(sound_count == 0, "a running reload timer plays no sound");
}

/* --- 3. an empty counter plays the empty sound and stops ------------------ */
static void test_empty_counter(void)
{
    reset();
    ctl.perSlot[0] = 0;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(sound_count == 1 && sound_log[0] == 0xD,
          "an empty counter plays the empty-clip sound");
    CHECK(fire_calls == 0, "an empty counter fires nothing");
}

/* --- 4. counter 5 plays the warning sound and still fires ----------------- */
static void test_warning_counter(void)
{
    reset();
    ctl.perSlot[0] = 5;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(sound_count == 1 && sound_log[0] == 0xE,
          "a counter of 5 plays the low-ammo sound");
    CHECK(fire_calls == 1, "a counter of 5 still fires");
}

/* --- 5. the counter is decremented after a successful shot --------------- */
static void test_counter_decrement(void)
{
    reset();
    ctl.perSlot[0] = 3;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.perSlot[0] == 2, "a successful shot decrements the counter");
}

/* --- 6. an invulnerable owner toggles instead of decrementing ------------ */
static void test_invulnerable_toggles(void)
{
    reset();
    ctl.perSlot[0] = 3;
    gPlayerInvulnerable[0] = 1;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.perSlot[0] == 2, "in bit 0 the counter toggles from odd to even");
    ctl.perSlot[0] = 4;
    ctl.reload[0] = 0;   /* the previous shot armed the reload timer */
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.perSlot[0] == 5, "in bit 0 the counter toggles from even to odd");
}

/* --- 7. a failed dispatch leaves the counter alone ----------------------- */
static void test_failed_dispatch(void)
{
    reset();
    ctl.perSlot[0] = 3;
    fire_result = 0;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.perSlot[0] == 3, "a failed dispatch does not consume the counter");
    CHECK(ctl.cycle[0] == 2, "a failed dispatch does not touch the cycle counter");
}

/* --- 8. the cycle counter reloads from the entry, else steps ------------ */
static void test_cycle_counter(void)
{
    reset();
    ctl.cycle[0] = 2;
    ctl.reload[0] = 0;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.cycle[0] == 1, "the cycle counter counts down");
    CHECK(ctl.reload[0] == 7, "a running cycle adds the entry's short delay");

    reset();
    ctl.cycle[0] = 1;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.cycle[0] == 9, "an expired cycle reloads from the entry's +0x06");
    CHECK(ctl.reload[0] == 4, "an expired cycle adds the entry's +0x08");
}

/* --- 9. the beam entry types go to the beam update ---------------------- */
static void test_beam_entry_routing(void)
{
    s32 ty;
    /* The gate is `ty >= 10 && (ty < 13 || ty == 14)`: 13 is NOT a beam type. */
    s32 types[5] = {10, 12, 13, 14, 9};
    s32 beam[5]  = {1, 1, 0, 1, 0};
    s32 i;

    for (i = 0; i < 5; i++) {
        ty = types[i];
        reset();
        registry_record.normal.type = ty;
        registry_record.normal.unk1F = 0x5A;
        gWeaponSlotUpdate(&obj, 0);
        if (beam[i]) {
            CHECK(beam_calls == 1 && fire_calls == 0, "beam entry types route to the beam update");
            CHECK(beam_last_type == 0x5A, "the beam update gets the entry's +0x1F byte");
        } else {
            CHECK(fire_calls == 1 && beam_calls == 0, "every other entry type fires the weapon");
        }
    }
}

/* --- 10. the shared counter drives the alternating muzzle swap ---------- */
static void test_alternating_muzzle(void)
{
    reset();
    ctl.secondary[0] = 1;              /* positive, so the shared counter is used */
    registry_record.unk44 = 0x1111;
    registry_record.unk50 = 0x2222;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.secondary[0] == 0, "the shared counter is cleared when it reaches zero");
    CHECK(registry_record.unk44 == 0x2222 && registry_record.unk50 == 0x1111,
          "the two muzzle halves swap after the shared counter runs out");
    /* The record byte at +0x0A is the same byte as entry +0x06 (the entry starts
     * at +0x04), so the reload value here is the entry's +0x06. */
    CHECK(ctl.cycle[0] == 9, "the cycle counter reloads from the record's +0x0A byte");
}

/* --- 11. a still positive shared counter does not swap ------------------ */
static void test_alternating_muzzle_kept(void)
{
    reset();
    ctl.secondary[0] = 3;
    registry_record.unk44 = 0x1111;
    registry_record.unk50 = 0x2222;
    gWeaponSlotUpdate(&obj, 0);
    CHECK(ctl.secondary[0] == 2, "the shared counter counts down");
    CHECK(registry_record.unk44 == 0x1111 && registry_record.unk50 == 0x2222,
          "the muzzle halves stay while the shared counter is positive");
}

/* --- 12. slot 2 retires the pending weapon ------------------------------ */
static void test_slot_two_retirement(void)
{
    struct WNode node;

    reset();
    node.next = 0;
    node.unk08 = 0x55;
    node.unk14 = 2;
    node.unk16 = 0xF;
    node.unk1C = 0;
    node.unkC4 = 9;
    ctl.pending = &node;
    ctl.pendingId = 0x55;
    gUnitListHead = &node;

    gWeaponSlotUpdate(&obj, 2);
    CHECK(ctl.pending == 0, "the pending node is cleared after the retirement pass");
    CHECK(node.unkC4 == 0, "the retired node of kind 0xF clears its +0xC4 word");
    CHECK(node.unk1C == 0, "the retired node of kind 0xF leaves +0x1C alone");
    CHECK(fire_calls == 0, "the retirement pass does not fire");
}


/* --- 13. the secondary counter is per slot, not shared ------------------- */
static void test_secondary_is_per_slot(void)
{
    reset();
    ctl.perSlot[0] = 2;
    ctl.perSlot[1] = 2;
    ctl.perSlot[2] = 2;
    ctl.secondary[0] = 2;
    ctl.secondary[1] = 2;
    ctl.secondary[2] = 2;

    gWeaponSlotUpdate(&obj, 1);

    /* Only slot 1 was driven, so only its own halfwords may move. The +0x8A
     * array is indexed by the slot index, not one shared word. */
    CHECK(ctl.secondary[0] == 2, "driving slot 1 leaves the slot-0 secondary counter");
    CHECK(ctl.secondary[2] == 2, "driving slot 1 leaves the slot-2 secondary counter");
    CHECK(ctl.perSlot[0] == 2, "driving slot 1 leaves the slot-0 counter");
    CHECK(ctl.perSlot[2] == 2, "driving slot 1 leaves the slot-2 counter");

    /* The read side must be indexed too: slot 1 must take its budget from its
     * own +0x8A halfword, not from the slot-0 one. Slot 0 is empty, slot 1 has
     * three rounds, and slot 1's +0x84 counter is empty, so a shared read would
     * fall through to the empty-clip sound instead of firing. */
    reset();
    ctl.secondary[0] = 0;
    ctl.secondary[1] = 3;
    ctl.secondary[2] = 0;
    ctl.perSlot[1] = 0;
    gWeaponSlotUpdate(&obj, 1);
    CHECK(fire_calls == 1, "slot 1 takes its budget from its own secondary counter");
    CHECK(sound_count == 0, "slot 1 does not fall back to the empty-clip sound");
    CHECK(ctl.secondary[1] == 2, "slot 1 decrements only its own secondary counter");
    CHECK(ctl.secondary[0] == 0, "slot 0's secondary counter stays untouched");
}

/* --- 14. a stale pending id clears and falls through to the normal path -- */
static void test_stale_pending_id(void)
{
    struct WNode node;

    reset();
    node.next = 0;
    node.unk08 = 0x11;
    node.unk14 = 2;
    node.unk16 = 0xF;
    node.unk1C = 0;
    node.unkC4 = 9;
    ctl.pending = &node;
    ctl.pendingId = 0x22;        /* does not match the node's +0x08 */
    gUnitListHead = &node;

    gWeaponSlotUpdate(&obj, 2);
    CHECK(ctl.pending == 0, "a stale pending id clears the pending pointer");
    CHECK(node.unkC4 == 9, "a stale pending id leaves the node alone");
    CHECK(node.unk1C == 0, "a stale pending id does not retire the node");
}

/* --- 15. a matched kind-7 node is retired via +0x1C --------------------- */
static void test_kind7_retirement(void)
{
    struct WNode node;

    reset();
    node.next = 0;
    node.unk08 = 0x55;
    node.unk14 = 2;
    node.unk16 = 7;
    node.unk1C = 0;
    node.unkC4 = 9;
    ctl.pending = &node;
    ctl.pendingId = 0x55;
    gUnitListHead = &node;

    gWeaponSlotUpdate(&obj, 2);
    CHECK(node.unk1C == -1, "a matched kind-7 node sets +0x1C to -1");
    CHECK(node.unkC4 == 9, "a matched kind-7 node leaves +0xC4 alone");
}

/* --- 16. an out-of-range negative index is rejected too ----------------- */
static void test_negative_index(void)
{
    reset();
    ctl.perSlot[0] = 0;
    gWeaponSlotUpdate(&obj, -1);
    CHECK(sound_count == 0 && fire_calls == 0,
          "a negative slot index is rejected like any other out-of-range index");
}

int main(void)
{
    test_index_gate();
    test_reload_gate();
    test_empty_counter();
    test_warning_counter();
    test_counter_decrement();
    test_invulnerable_toggles();
    test_failed_dispatch();
    test_cycle_counter();
    test_beam_entry_routing();
    test_alternating_muzzle();
    test_alternating_muzzle_kept();
    test_slot_two_retirement();
    test_secondary_is_per_slot();
    test_stale_pending_id();
    test_kind7_retirement();
    test_negative_index();

    if (failures == 0) say("weapon animation: all checks passed\n");
    return failures;
}
