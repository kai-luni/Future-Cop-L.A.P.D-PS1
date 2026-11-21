/* Behavioural checks for the weapon input poller (func_80055E10).
 *
 * The poller and the real four-phase edge decoder (func_8005780C) are compiled
 * from the unchanged decomp sources and linked here, so these checks run the
 * actual code path that turns pad bits into weapon-slot calls. Only the two
 * callees are stubbed: the lock-on acquisition and the slot driver.
 *
 * What is pinned down: the pause byte gates everything, the slot bits select
 * slots independently, slot 2 fires on a phase match rather than a press edge,
 * and the lock is only refreshed on the press edge. */
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

struct InputAction {
    u32 buttons;
    s32 phase;
};

struct WIControl {
    u8 pad00[0x30];
    u32 buttons;
    struct InputAction fire;
    struct InputAction special;
    u8 pad44[8];
    struct InputAction weapon;
    u8 pad54[0x51];
    u8 weaponState;
    u8 padA6[0x26A];
    s8 locked;
};

struct WIObject {
    u8 pad00[0xAC];
    struct WIControl *control;
};

/* --- the units under test ------------------------------------------------ */
void gWeaponSlotInput(struct WIObject *object) __asm__("func_80055E10");
void gDecodeButtonEdge(u32 buttons, struct InputAction *action) __asm__("func_8005780C");

/* --- the two stubbed callees, recording what they receive ---------------- */
static s32 slot_log[8];
static s32 slot_count;
static s32 lock_calls;
static s32 lock_arg;

void gAcquirePlayerLockOnTarget(struct WIObject *object, s32 arg1) __asm__("func_8006C34C");
void gAcquirePlayerLockOnTarget(struct WIObject *object, s32 arg1)
{
    (void)object;
    lock_calls++;
    lock_arg = arg1;
}

void gWeaponSlotUpdate(struct WIObject *object, s32 idx) __asm__("func_8006BA50");
void gWeaponSlotUpdate(struct WIObject *object, s32 idx)
{
    (void)object;
    if (slot_count < 8) slot_log[slot_count++] = idx;
}

/* --- fixtures ------------------------------------------------------------ */
static struct WIControl ctl;
static struct WIObject object;

#define SLOT0_BIT 0x04000000
#define SLOT1_BIT 0x00400000

static void reset(void)
{
    u8 *p = (u8 *)&ctl;
    s32 i;

    for (i = 0; i < (s32)sizeof(ctl); i++) p[i] = 0;
    object.control = &ctl;
    slot_count = 0;
    lock_calls = 0;
    lock_arg = -1;
    /* Actor reset leaves the action phases idle at 2. */
    ctl.fire.buttons = 0x00800000;
    ctl.fire.phase = 2;
    ctl.special.buttons = 0x02000000;
    ctl.special.phase = 2;
    ctl.weapon.buttons = 0x01000000;
    ctl.weapon.phase = 2;
    ctl.weaponState = 3;
    ctl.locked = 0;
    ctl.buttons = 0;
}

/* --- 1. the pause byte gates the whole poller ---------------------------- */
static void test_pause_gate(void)
{
    reset();
    ctl.locked = 1;
    ctl.buttons = SLOT0_BIT | SLOT1_BIT;
    ctl.fire.phase = 2;

    gWeaponSlotInput(&object);
    CHECK(slot_count == 0, "a set pause byte stops every slot call");
    CHECK(lock_calls == 0, "a set pause byte stops the lock refresh");
    CHECK(ctl.fire.phase == 2, "a set pause byte leaves the action phases alone");
}

/* --- 2. the pause byte is the one at +0x310 ----------------------------- */
static void test_pause_offset(void)
{
    /* The structure is 0x311 bytes and `locked` is its last byte, so the field
     * really sits at +0x310. A stale doc block once put it at +0x2C8, which is
     * inside the pad run - writing there must not gate the poller, and writing
     * the last byte must. */
    u8 *base = (u8 *)&ctl;

    reset();
    ctl.buttons = SLOT0_BIT;
    base[0x2C8] = 1;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 1, "a byte at +0x2C8 does not gate the poller");

    reset();
    ctl.buttons = SLOT0_BIT;
    base[0x310] = 1;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 0, "the byte at +0x310 is the pause gate");

    /* And the field the source names really lands on that byte. */
    CHECK((u32)((u8 *)&ctl.locked - (u8 *)&ctl) == 0x310,
          "the locked field sits at +0x310");

}

/* --- 3. slots 0 and 1 follow their own bits ---------------------------- */
static void test_slot_bits(void)
{
    reset();
    ctl.buttons = SLOT0_BIT;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 1 && slot_log[0] == 0, "the slot-0 bit calls slot 0 only");

    reset();
    ctl.buttons = SLOT1_BIT;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 1 && slot_log[0] == 1, "the slot-1 bit calls slot 1 only");

    reset();
    ctl.buttons = SLOT0_BIT | SLOT1_BIT;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 2, "both bits call both slots");
    CHECK(slot_log[0] == 0 && slot_log[1] == 1, "slots are called in ascending order");

    reset();
    ctl.buttons = 0;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 0, "no slot bit calls no slot");
}

/* --- 4. slot 2 is a phase match, not a bit ----------------------------- */
static void test_slot_two_phase(void)
{
    /* Hold the weapon button: the decoder takes the action from idle 2 to the
     * press phase 0 first, so state 3 is reached on the second held frame. */
    reset();
    ctl.buttons = ctl.weapon.buttons;
    ctl.weaponState = 3;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 0, "the press phase 0 does not match state 3");
    slot_count = 0;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 1 && slot_log[0] == 2,
          "the held phase 3 matches state 3 and calls slot 2");

    reset();
    ctl.weapon.phase = 1;      /* release phase, decoded back to idle below */
    ctl.weaponState = 3;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 0, "a different phase does not call slot 2");

    /* With the state at 0 the held phase 3 does not match, but the press
     * phase of the next frame does - the same code serves both. */
    /* State 0 is the press phase: a fresh press must call slot 2 at once. */
    reset();
    ctl.buttons = ctl.weapon.buttons;
    ctl.weaponState = 0;
    gWeaponSlotInput(&object);
    CHECK(slot_count == 1 && slot_log[0] == 2,
          "state 0 matches the press phase and calls slot 2");
}

/* --- 5. the lock is refreshed on the press edge only ------------------- */
static void test_lock_on_press_edge(void)
{
    reset();
    ctl.buttons = ctl.fire.buttons;     /* fire pressed, phase 2 -> 0 */
    gWeaponSlotInput(&object);
    CHECK(lock_calls == 1, "a fire press refreshes the lock");
    CHECK(lock_arg == 0, "the lock refresh passes argument 0");
    CHECK(ctl.fire.phase == 0, "the fire press reaches the press phase");

    /* Next frame, still held: no second lock refresh. */
    lock_calls = 0;
    slot_count = 0;
    gWeaponSlotInput(&object);
    CHECK(lock_calls == 0, "a held fire button does not refresh the lock again");
    CHECK(ctl.fire.phase == 3, "the held fire button reaches the held phase");
}

/* --- 6. the real decoder drives the phases ----------------------------- */
static void test_decoder_phases(void)
{
    struct InputAction action;

    /* idle -> press -> held -> release -> idle */
    action.buttons = 0x10;
    action.phase = 2;
    gDecodeButtonEdge(0x10, &action);
    CHECK(action.phase == 0, "a press from idle gives the press phase");
    gDecodeButtonEdge(0x10, &action);
    CHECK(action.phase == 3, "a held button gives the held phase");
    gDecodeButtonEdge(0x00, &action);
    CHECK(action.phase == 1, "a release gives the release phase");
    gDecodeButtonEdge(0x00, &action);
    CHECK(action.phase == 2, "a released button returns to idle");

    /* an unrelated bit is not a press */
    action.phase = 2;
    gDecodeButtonEdge(0x20, &action);
    CHECK(action.phase == 2, "an unrelated bit leaves the phase idle");
}

/* --- 7. every slot is decoded, the fire one only refreshes the lock ---- */
static void test_all_three_decoded(void)
{
    reset();
    ctl.buttons = ctl.special.buttons | ctl.weapon.buttons;
    gWeaponSlotInput(&object);
    CHECK(ctl.special.phase == 0, "the special action is decoded");
    CHECK(ctl.weapon.phase == 0, "the weapon action is decoded");
    CHECK(ctl.fire.phase == 2, "the untouched fire action stays idle");
    CHECK(lock_calls == 0, "no fire press means no lock refresh");
}

int main(void)
{
    test_pause_gate();
    test_pause_offset();
    test_slot_bits();
    test_slot_two_phase();
    test_lock_on_press_edge();
    test_decoder_phases();
    test_all_three_decoded();

    if (failures == 0) say("weapon input: all checks passed\n");
    return failures;
}
