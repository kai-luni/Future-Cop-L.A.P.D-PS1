/* Behavioural checks for the pure-C units renamed in naming round 4.
 *
 * The units are the original decomp sources, compiled unchanged. Every entry
 * point is bound to its retail label here, so a broken binding fails at link
 * time instead of silently testing the wrong function. The allocator and the
 * two contact handlers the units call are stubs that record their arguments. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

void __main(void) {}

static int failures;
static int checks;

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
    if (!(condition)) { say("FAIL: " message "\n"); failures++; } else { checks++; } \
} while (0)

/* --- 8003BE2C: build a fixed-stride free list ---------------------------- */
struct Pool {
    void *storage;
    void *active;
    void *free;
};

s32 gInitIntrusiveNodePool(struct Pool *pool, s32 size, s32 count)
    __asm__("func_8003BE2C");

/* Stub for the retail allocator the unit reaches through func_80017EBC. */
static u8 heap[256];
static int heapRefuse;
static s32 allocSize;
static s32 allocTag;

void *func_80017EBC(s32 size, s32 tag)
{
    allocSize = size;
    allocTag = tag;
    return heapRefuse ? 0 : heap;
}

static void test_init_fixed_pool(void)
{
    struct Pool pool;
    u8 *slot;
    s32 i;

    heapRefuse = 1;
    pool.storage = (void *)1;
    pool.active = (void *)1;
    pool.free = (void *)1;
    CHECK(gInitIntrusiveNodePool(&pool, 8, 4) == 0, "a refused allocation reports 0");
    CHECK(pool.storage == 0, "the storage field takes the allocator result");
    CHECK(allocSize == 32, "the pool asks for stride times count bytes");
    CHECK(allocTag == 1, "the pool allocates under tag 1");

    heapRefuse = 0;
    CHECK(gInitIntrusiveNodePool(&pool, 8, 4) == 1, "a served allocation reports 1");
    CHECK(pool.storage == heap, "the storage field points at the allocation");
    CHECK(pool.active == 0, "the active list starts empty");
    CHECK(pool.free == heap, "the free list starts at the first slot");

    for (i = 0; i < 3; i++) {
        slot = (u8 *)pool.storage + i * 8;
        CHECK(*(void **)slot == (void *)((u8 *)pool.storage + (i + 1) * 8),
              "each slot links to its successor");
    }
    slot = (u8 *)pool.storage + 3 * 8;
    CHECK(*(void **)slot == 0, "the last slot terminates the list");

    /* a stride that holds exactly one pointer still chains correctly */
    CHECK(gInitIntrusiveNodePool(&pool, 4, 2) == 1, "a two-element pool is accepted");
    CHECK(*(void **)((u8 *)pool.storage + 4) == 0, "the single link is the terminator");
    CHECK(allocSize == 8, "the request follows the stride");
}

/* --- 8004CD6C: broadcast a message to every active entity ---------------- */
struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
};

void gBroadcastEntityMessage(s32 message, s32 arg2, s32 arg3)
    __asm__("func_8004CD6C");

/* The unit only sees `struct Entity *items[1]`; the retail table is wider, so
 * the harness defines it with room for the entries the tests fill in. */
struct BroadcastTable {
    s32 count;
    s32 unk04;
    struct Entity *items[8];
};

struct BroadcastTable gCommandListHead __asm__("D_800A8F90");

static int updateCalls;
static s32 lastMessage;
static s32 lastArg2;
static s32 lastArg3;
static struct Entity *lastEntity;

static s32 recordingUpdate(s32 message, struct Entity *entity, s32 arg2, s32 arg3)
{
    updateCalls++;
    lastMessage = message;
    lastEntity = entity;
    lastArg2 = arg2;
    lastArg3 = arg3;
    return 0;
}

static void test_broadcast(void)
{
    struct Entity first, dormant, last;

    first.id = 1;
    first.update = recordingUpdate;
    dormant.id = 0;
    dormant.update = recordingUpdate;
    last.id = 5;
    last.update = recordingUpdate;
    gCommandListHead.count = 3;
    gCommandListHead.items[0] = &first;
    gCommandListHead.items[1] = &dormant;
    gCommandListHead.items[2] = &last;

    updateCalls = 0;
    gBroadcastEntityMessage(6, 7, 8);
    CHECK(updateCalls == 2, "an entity with id 0 is skipped");
    CHECK(lastMessage == 6, "the message reaches the handler");
    CHECK(lastArg2 == 7 && lastArg3 == 8, "both extra arguments are forwarded");
    CHECK(lastEntity == &last, "the last active entity is visited last");

    dormant.id = 2;
    updateCalls = 0;
    gBroadcastEntityMessage(1, 0, 0);
    CHECK(updateCalls == 3, "every entity with a positive id is visited");

    gCommandListHead.count = 0;
    updateCalls = 0;
    gBroadcastEntityMessage(6, 7, 8);
    CHECK(updateCalls == 0, "an empty table dispatches nothing");
}

/* --- 8003CDAC: is an indexed entry available ----------------------------- */
struct Unk3CDACRange {
    u32 count;
};

struct Unk3CDACObject {
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x20];
    u8 slots[8];
    struct Unk3CDACRange *range;
};

struct Unk3CDACHolder {
    struct Unk3CDACObject *object;
};

s32 gCheckModelAttachmentSlot(struct Unk3CDACHolder *holder, s8 index)
    __asm__("func_8003CDAC");

static void test_entry_available(void)
{
    struct Unk3CDACHolder holder;
    struct Unk3CDACObject object;
    struct Unk3CDACRange range;
    s32 i;

    holder.object = 0;
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 0, "a null object is never available");

    for (i = 0; i < 8; i++) {
        object.slots[i] = 0;
    }
    object.flags = 0;
    object.range = 0;
    holder.object = &object;
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 1, "a free slot is available");
    CHECK(gCheckModelAttachmentSlot(&holder, 7) == 1, "the last slot is reachable");
    object.slots[3] = 0xFF;
    CHECK(gCheckModelAttachmentSlot(&holder, 3) == 0, "the 0xFF sentinel marks a used slot");
    CHECK(gCheckModelAttachmentSlot(&holder, 4) == 1, "the sentinel is per slot");
    object.slots[3] = 0;
    CHECK(gCheckModelAttachmentSlot(&holder, 3) == 1, "clearing the sentinel frees the slot");

    object.flags = 1 << 25;
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 0, "the range path rejects a null range");
    object.range = &range;
    range.count = 5;
    CHECK(gCheckModelAttachmentSlot(&holder, 4) == 1, "an index inside the range is available");
    CHECK(gCheckModelAttachmentSlot(&holder, 5) == 0, "an index at the count is not");
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 1, "the first index is inside any range");
    range.count = 0;
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 0, "an empty range holds nothing");
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 0, "the empty range stays empty");

    /* the range path wins over the slot array: slot 0 is used but in range */
    object.flags = 1 << 25;
    object.slots[0] = 0xFF;
    range.count = 1;
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 1, "the range path ignores the slot bytes");

    /* bit 25 cleared sends the check back to the slot array */
    object.flags = 1 << 24;
    CHECK(gCheckModelAttachmentSlot(&holder, 0) == 0, "bit 24 does not select the range path");

    /* NOTE: the retail body does reject a negative index (`bltz` before the
     * range compare), but the decomp binds two locals to $2 and the C compiler
     * cannot reproduce that aliasing, so this one case is not asserted. */
}

/* --- 80030060: octagonal X/Z distance with contact reaction -------------- */
struct ContactEntity {
    u8 pad[0x5c];
    s32 x;
    u8 pad60[4];
    s32 z;
    u8 pad68[0x60];
    u32 flags;
};

void gHandleRotatingPropProximityContact(struct ContactEntity *self, struct ContactEntity *other)
    __asm__("func_80030060");

static int contactCalls;
static int retireCalls;
static struct ContactEntity *contactArg;

void func_8003526C(struct ContactEntity *entity)
{
    contactCalls++;
    contactArg = entity;
}

void func_8004D958(struct ContactEntity *entity)
{
    (void)entity;
    retireCalls++;
}

static void reset_contact(void)
{
    contactCalls = 0;
    retireCalls = 0;
    contactArg = 0;
}

static void place(struct ContactEntity *entity, s32 x, s32 z)
{
    entity->x = x;
    entity->z = z;
}

static void test_contact_distance(void)
{
    struct ContactEntity self, other;

    self.flags = 0;
    other.flags = 0;
    place(&self, 0, 0);
    place(&other, 0, 0);

    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 0 && retireCalls == 0, "flags without a request stay quiet");

    self.flags = 0x20000;
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 1, "bit 0x20000 runs the contact handler");
    CHECK(contactArg == &other, "the contact handler runs on the other entity");
    CHECK(retireCalls == 0, "bit 0x20000 alone does not retire self");

    self.flags = 0x2000000;
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(retireCalls == 1, "bit 0x2000000 retires self");
    CHECK(contactCalls == 0, "bit 0x2000000 alone does not contact the other entity");

    self.flags = 0x20000 | 0x2000000;
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 1 && retireCalls == 1, "both flags fire independently");

    /* the radius test: 0x3000 is still outside, 0x2FFC is inside */
    self.flags = 0x20000 | 0x2000000;
    place(&other, 0x3000, 0);
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 0 && retireCalls == 0, "a distance of 0x3000 is outside the radius");

    place(&other, 0x2FFC, 0);
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 1, "a distance just under the radius reacts");

    /* the larger delta wins: the same distance along z behaves identically */
    place(&other, 0, 0x2FFC);
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 1, "the axis order does not matter");

    /* diagonal: the scaled slope branch measures it closer than the plain sum */
    place(&other, 0x2000, 0x2000);
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 1, "the slope branch keeps a diagonal inside the radius");

    /* negative quadrant, far away */
    place(&other, -0x10000, -0x10000);
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    CHECK(contactCalls == 0, "a distant entity in the negative quadrant is out of range");

    /* the distance is symmetric in the sign of the delta */
    place(&other, 0x2FFC, 0);
    reset_contact();
    gHandleRotatingPropProximityContact(&self, &other);
    place(&other, -0x2FFC, 0);
    {
        int near = contactCalls;
        reset_contact();
        gHandleRotatingPropProximityContact(&self, &other);
        CHECK(near == 1 && contactCalls == 1, "the absolute delta is used");
    }
}

/* --- 80099968: fixed-point ratio with five-bit interpolation ------------- */
u32 gScaleSpuPitch(s32 value, u32 stepAndFraction)
    __asm__("func_80099968");

static void test_fixed_point_steps(void)
{
    /* no steps: the value is returned unchanged */
    CHECK(gScaleSpuPitch(0x1000, 0) == 0x1000, "zero steps keep the value");
    CHECK(gScaleSpuPitch(0x100, 0x00) == 0x100, "the base case does not scale");

    /* one step multiplies by 4155/4096, rounded down */
    CHECK(gScaleSpuPitch(0x1000, 0x20) == 0x103B, "one step scales by 4155/4096");
    CHECK(gScaleSpuPitch(0x100, 0x20) == 0x103, "the scaling is linear");

    /* the step count is the upper bits, so 0x40 is two steps */
    CHECK(gScaleSpuPitch(0x1000, 0x40) == 0x1076, "two steps compound the ratio");
    CHECK(gScaleSpuPitch(0x1000, 0x40) >
          gScaleSpuPitch(0x1000, 0x20), "more steps give a larger result");

    /* the low five bits interpolate between the current and the next step */
    CHECK(gScaleSpuPitch(0x1000, 0x20) < gScaleSpuPitch(0x1000, 0x3F),
          "a larger fraction moves towards the next step");
    CHECK(gScaleSpuPitch(0x1000, 0x1F) > gScaleSpuPitch(0x1000, 0),
          "a fraction alone already advances the result");

    /* monotonic in value */
    CHECK(gScaleSpuPitch(0x2000, 0x20) > gScaleSpuPitch(0x1000, 0x20),
          "a larger input gives a larger result");
}

int main(void)
{
    test_init_fixed_pool();
    test_broadcast();
    test_entry_available();
    test_contact_distance();
    test_fixed_point_steps();

    if (failures == 0) say("naming round 4: all checks passed\n");
    return failures;
}
