/* Behavioural checks for the pure arithmetic, list and bit-manipulation units
 * renamed in naming round 3.
 *
 * The units are the original decomp sources, compiled unchanged. Each entry
 * point is declared here under its readable name with the retail label, so a
 * broken label binding fails at link time. */
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

/* --- 80092324: set bit 31, keep the low 24 bits --------------------------- */
u32 func_80092324(u32 *value);

/* --- 8002B1D0: copy the reload halfword into the countdown field ---------- */
struct ValuePair {
    unsigned char pad[0x10C];
    u16 source;
    u16 destination;
};

void gCopyPairSourceToDestination(struct ValuePair *value)
    __asm__("func_8002B1D0");

/* --- 8004AAC0: link a node behind another one ---------------------------- */
struct LinkNode {
    unsigned char pad00[0x48];
    struct LinkNode *back;
    unsigned char pad4C[0x38];
    struct LinkNode *forward;
};

void gInsertNodeAfter(struct LinkNode *left, struct LinkNode *right)
    __asm__("func_8004AAC0");

/* --- 800999F4: SPU note pair -> pitch ------------------------------------ */
u16 gConvertSpuNoteToPitch(u16 a0, u16 a1, u16 a2, u16 a3)
    __asm__("func_800999F4");

static int failures;
static int checks;

void __main(void) {}

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

static void fill(char *raw, int size)
{
    int i;
    for (i = 0; i < size; i++) raw[i] = 0;
}

/* The unit keeps the top bit and the low three bytes and drops bits 24..30. */
static void test_high_bit_mask(void)
{
    u32 value;

    value = 0x00000000;
    CHECK(func_80092324(&value) == 0x80000000U, "zero sets bit 31");
    CHECK(value == 0, "the pointed-to word stays untouched");

    value = 0xFFFFFFFFU;
    CHECK(func_80092324(&value) == 0x80FFFFFFU, "all ones keep the low 24 bits");

    value = 0x01234567;
    CHECK(func_80092324(&value) == 0x80234567U, "bits 24..30 are cleared");

    value = 0x7F000000;
    CHECK(func_80092324(&value) == 0x80000000U, "bits 24..30 alone vanish");

    value = 0x80000000U;
    CHECK(func_80092324(&value) == 0x80000000U, "bit 31 already set stays set");
}

/* The unit copies +0x10C into +0x10E and touches nothing else. */
static void test_reload_copy(void)
{
    struct ValuePair pair;
    struct ValuePair untouched;

    fill((char *)&pair, (int)sizeof(pair));
    fill((char *)&untouched, (int)sizeof(untouched));
    pair.source = 0x1234;
    pair.destination = 0xFFFF;
    gCopyPairSourceToDestination(&pair);
    CHECK(pair.destination == 0x1234, "the reload value reaches the countdown field");
    CHECK(pair.source == 0x1234, "the source field is unchanged");

    pair.source = 0xABCD;
    gCopyPairSourceToDestination(&pair);
    CHECK(pair.destination == 0xABCD, "a second copy picks up the new reload value");

    pair.source = 0;
    pair.destination = 0x5555;
    gCopyPairSourceToDestination(&pair);
    CHECK(pair.destination == 0, "a zero reload value is copied as well");

    /* the write stays inside the copied halfword: neighbours keep their bytes */
    pair.source = 0x4242;
    pair.destination = 0x0000;
    *(u16 *)((char *)&pair + 0x10A) = 0x1111;
    *(u16 *)((char *)&pair + 0x110) = 0x2222;
    gCopyPairSourceToDestination(&pair);
    CHECK(pair.destination == 0x4242, "the copy lands in the countdown field");
    CHECK(*(u16 *)((char *)&pair + 0x10A) == 0x1111, "the field before is untouched");
    CHECK(*(u16 *)((char *)&pair + 0x110) == 0x2222, "the field after is untouched");
}

/* The unit splices a node behind another one and only repairs the new back link. */
static void test_insert_node(void)
{
    struct LinkNode left, right, successor;

    fill((char *)&left, (int)sizeof(left));
    fill((char *)&right, (int)sizeof(right));
    fill((char *)&successor, (int)sizeof(successor));

    left.forward = &successor;
    successor.back = &left;
    gInsertNodeAfter(&left, &right);
    CHECK(left.forward == &right, "the new node becomes the successor");
    CHECK(right.back == &successor, "the new node copies the old forward pointer");
    CHECK(successor.back == &left, "the old successor keeps its stale back link");

    /* an empty slot behind the insertion point is fine too */
    left.forward = 0;
    right.back = &successor;
    gInsertNodeAfter(&left, &right);
    CHECK(left.forward == &right, "insertion works at the end of the chain");
    CHECK(right.back == 0, "the new node copies the empty forward pointer");

    /* the link is a plain pointer copy, not a splice of the old chain */
    left.forward = &successor;
    successor.back = &right;
    gInsertNodeAfter(&left, &right);
    CHECK(right.back == &successor, "the back pointer mirrors the old forward pointer");
    CHECK(successor.back == &right, "the successor keeps the value it was given");
}

/* The unit interpolates a pitch between two (note, fraction) pairs. */
static void test_pitch_conversion(void)
{
    /* identical pairs: the table entry for fraction 0 is returned */
    CHECK(gConvertSpuNoteToPitch(0, 0, 0, 0) == 0x1000, "equal notes give the base pitch");
    CHECK(gConvertSpuNoteToPitch(3, 7, 3, 7) == 0x1000, "equal pairs give the base pitch");

    /* one whole note up is one semitone: 0x1000 * 4155 >> 12 */
    CHECK(gConvertSpuNoteToPitch(0, 0, 0, 0x600) == 0x2000, "one note up doubles the pitch");
    CHECK(gConvertSpuNoteToPitch(0, 0, 0, 0x300) == 0x1681, "half a note up is tone + half");
    CHECK(gConvertSpuNoteToPitch(0, 0, 0, 0x600 * 2) == 0x3FFF, "two notes up saturate");
    CHECK(gConvertSpuNoteToPitch(0, 0, 12, 0) == 0x2000, "the coarse field counts 128 fine units");
    CHECK(gConvertSpuNoteToPitch(0, 0, 6, 0) == 0x1681, "a coarse step and a half step agree");

    /* downwards mirrors the table: 0x600 fine units are twelve coarse steps */
    CHECK(gConvertSpuNoteToPitch(0x600, 0, 0, 0) == 0x1000, "one note down returns the base pitch");
    CHECK(gConvertSpuNoteToPitch(0x600, 0, 0x600, 0) == 0x1000, "equal notes are direction independent");
    CHECK(gConvertSpuNoteToPitch(12, 0, 0, 0) == 0x800, "a full note down halves the pitch");
    CHECK(gConvertSpuNoteToPitch(8, 0, 0, 0) == 0xA0A, "a partial note down lowers the pitch");

    /* the high bank shifts the note by 0x80 steps, matching the packed format */
    CHECK(gConvertSpuNoteToPitch(0, 0, 1, 0) == 0x10EF, "the high bank scales the note");
    CHECK(gConvertSpuNoteToPitch(1, 0, 0, 0) == 0xEF4, "the split note one step up is mirrored");
    CHECK(gConvertSpuNoteToPitch(0, 0x80, 0, 0) == 0xEF4, "a fine step up matches the coarse one");

    /* monotonic in the target note */
    CHECK(gConvertSpuNoteToPitch(0, 0, 2, 0) < gConvertSpuNoteToPitch(0, 0, 4, 0),
          "a higher target note gives a higher pitch");
    CHECK(gConvertSpuNoteToPitch(0, 0, 4, 0) < gConvertSpuNoteToPitch(0, 0, 8, 0),
          "the pitch keeps rising with the note");
}

int main(void)
{
    test_high_bit_mask();
    test_reload_copy();
    test_insert_node();
    test_pitch_conversion();
    if (failures == 0) say("naming round 3: all checks passed\n");
    return failures;
}
