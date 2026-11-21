/* Behavioural checks for the units renamed in naming round 6.
 *
 * The units are the original decomp sources, compiled unchanged. Every entry
 * point is bound to its retail label here, so a broken binding fails at link
 * time instead of silently testing the wrong function. Units that only touch a
 * struct are driven through a byte-for-byte compatible local layout. */
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

/* --- 80092324: advance to the next primitive ---------------------------- */
u32 gGetNextPrim(u32 *value) __asm__("func_80092324");

static void test_set_top_bit(void)
{
    u32 value;

    value = 0x00123456;
    CHECK(gGetNextPrim(&value) == 0x80123456, "the top bit is set");
    CHECK(value == 0x00123456, "the source word is left alone");

    value = 0xFFFFFFFF;
    CHECK(gGetNextPrim(&value) == 0x80FFFFFF, "an already set bit stays set");

    value = 0x00000000;
    CHECK(gGetNextPrim(&value) == 0x80000000, "only the top bit appears");

    value = 0xFF000000;
    CHECK(gGetNextPrim(&value) == 0x80000000, "bits above 24 are cleared first");
}

/* --- 80073BA8: raise a flag bit ------------------------------------------ */
struct FlagValue {
    u8 pad00[0x0E];
    u16 flags;
};

void gSetObjectFlag1000(struct FlagValue *value) __asm__("func_80073BA8");

static void test_raise_flag(void)
{
    struct FlagValue value;

    value.flags = 0;
    gSetObjectFlag1000(&value);
    CHECK(value.flags == 0x1000, "the bit is set on a clear word");

    value.flags = 0x1001;
    gSetObjectFlag1000(&value);
    CHECK(value.flags == 0x1001, "an already set bit stays set");

    value.flags = 0x0FFF;
    gSetObjectFlag1000(&value);
    CHECK(value.flags == 0x1FFF, "the surrounding bits are kept");

    value.flags = 0xEFFF;
    gSetObjectFlag1000(&value);
    CHECK(value.flags == 0xFFFF, "only bit 0x1000 is touched");
}

/* --- 80092460 / 80092474 / 80092500: fixed slot type and size pairs ------ */
void gSetFlatTrianglePacketHeader(u8 *p) __asm__("func_80092460");
void gSetPolyFT3Packet(u8 *p) __asm__("func_80092474");
void gSetSprt8(u8 *p) __asm__("func_80092500");

static void test_slot_pairs(void)
{
    u8 buffer[8];
    int i;

    for (i = 0; i < 8; i++) buffer[i] = 0xAA;

    gSetFlatTrianglePacketHeader(buffer);
    CHECK(buffer[3] == 4 && buffer[7] == 0x20, "the 4 / 0x20 pair is written");
    CHECK(buffer[2] == 0xAA && buffer[4] == 0xAA, "neighbour bytes are untouched");

    for (i = 0; i < 8; i++) buffer[i] = 0;
    gSetPolyFT3Packet(buffer);
    CHECK(buffer[3] == 7 && buffer[7] == 36, "the 7 / 36 pair is written");

    for (i = 0; i < 8; i++) buffer[i] = 0;
    gSetSprt8(buffer);
    CHECK(buffer[3] == 3 && buffer[7] == 116, "the 3 / 116 pair is written");
}

/* --- 80092614 / 800925B4: preset three state fields ---------------------- */
struct PresetState {
    u8 pad0[3];
    u8 field3;
    u8 pad4[3];
    u8 field7;
    u8 pad8[28];
    u32 field24;
};

struct ShortPresetState {
    u8 pad0[3];
    u8 field3;
    u8 pad4[3];
    u8 field7;
    u8 pad8[12];
    u32 field14;
};

void gInitGouraudFourPointPolylinePacket(struct PresetState *state) __asm__("func_80092614");
void gInitFlatThreePointPolylinePacket(struct ShortPresetState *state) __asm__("func_800925B4");

static void test_state_presets(void)
{
    struct PresetState state;
    u8 *bytes;
    int i;

    for (i = 0; i < (int)sizeof state; i++) ((u8 *)&state)[i] = 0xEE;

    gInitGouraudFourPointPolylinePacket(&state);
    CHECK(state.field3 == 9, "the first field gets 9");
    CHECK(state.field7 == 0x5c, "the second field gets 0x5c");
    CHECK(state.field24 == 0x55555555, "the third field gets the pattern");
    bytes = (u8 *)&state;
    CHECK(bytes[0] == 0xEE && bytes[4] == 0xEE, "the padding is preserved");

    {
        struct ShortPresetState shortState;

        for (i = 0; i < (int)sizeof shortState; i++) ((u8 *)&shortState)[i] = 0xEE;
        gInitFlatThreePointPolylinePacket(&shortState);
        CHECK(shortState.field3 == 5, "the second preset writes 5");
        CHECK(shortState.field7 == 0x48, "the second preset writes 0x48");
        CHECK(shortState.field14 == 0x55555555, "the second preset writes the pattern");
    }
}

/* --- 8009272C: advance a shared counter with an upper bound -------------- */
s32 gCatPrimPackets(u8 *a0, u8 *a1) __asm__("func_8009272C");

static void test_bounded_counter(void)
{
    u8 a0[4];
    u8 a1[4];
    int i;

    for (i = 0; i < 4; i++) { a0[i] = 0; a1[i] = 0xFF; }
    a0[3] = 0x0F;
    a1[3] = 0x02;

    /* 0x0F + 0x02 + 1 = 0x12, above the cap of 0x11 */
    CHECK(gCatPrimPackets(a0, a1) == -1, "a sum above 0x11 is rejected");
    CHECK(a0[3] == 0x0F, "a rejected sum leaves the counter alone");

    a0[3] = 0x0E;
    a1[3] = 0x02;
    CHECK(gCatPrimPackets(a0, a1) == -1, "a sum of exactly 0x11 is still rejected");
    CHECK(a0[3] == 0x0E, "the rejected step leaves the counter alone");

    a0[3] = 0x0D;
    a1[3] = 0x02;
    CHECK(gCatPrimPackets(a0, a1) == 0, "a sum of 0x10 is the largest accepted one");
    CHECK(a0[3] == 0x10, "the counter advances by the step plus one");
    CHECK(*(u32 *)a1 == 0, "the step word is cleared after a success");

    a0[3] = 0;
    a1[3] = 0;
    CHECK(gCatPrimPackets(a0, a1) == 0, "a zero step is accepted");
    CHECK(a0[3] == 1, "a zero step still advances by one");
}

/* --- 800489EC: text code to packet size ---------------------------------- */
s32 gGetFontBitsPerPixel(u8 *value) __asm__("func_800489EC");

static void test_text_size(void)
{
    u8 value;

    value = 0x40;
    CHECK(gGetFontBitsPerPixel(&value) == 4, "code 0x40 needs four bytes");
    value = 0x44;
    CHECK(gGetFontBitsPerPixel(&value) == 1, "code 0x44 needs one byte");
    value = 0x41;
    CHECK(gGetFontBitsPerPixel(&value) == 4, "an unknown code falls back to four bytes");
    value = 0xFF;
    CHECK(gGetFontBitsPerPixel(&value) == 4, "the fallback does not depend on the value");
}

/* --- 80091F38: zero a word array ----------------------------------------- */
void gZeroIntArray(s32 *destination, u32 count) __asm__("func_80091F38");

static void test_zero_words(void)
{
    s32 buffer[5];

    buffer[0] = 1; buffer[1] = 2; buffer[2] = 3; buffer[3] = 4; buffer[4] = 5;
    gZeroIntArray(buffer, 3);
    CHECK(buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0, "the first three words are cleared");
    CHECK(buffer[3] == 4 && buffer[4] == 5, "the words past the count are kept");

    buffer[0] = 9;
    gZeroIntArray(buffer, 0);
    CHECK(buffer[0] == 9, "a zero count writes nothing");
}

/* --- 8007F5C8: constant accessor ----------------------------------------- */
s32 gQueryFrontEndVerticalRepeatMask(void) __asm__("func_8007F5C8");

static void test_constant(void)
{
    CHECK(gQueryFrontEndVerticalRepeatMask() == 2, "the accessor returns 2");
}

int main(void)
{
    test_set_top_bit();
    test_raise_flag();
    test_slot_pairs();
    test_state_presets();
    test_bounded_counter();
    test_text_size();
    test_zero_words();
    test_constant();

    if (failures == 0) say("naming round 6: all checks passed\n");
    return failures;
}
