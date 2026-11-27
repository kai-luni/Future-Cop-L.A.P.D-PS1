/* ISO-9660 path formatting through the shared BIOS mock.
 *
 * Unit under test (unchanged decomp source, linked verbatim):
 *   func_8001DC48  gFormatIso9660Path(mode, name, dir, out)
 *
 * Why this unit is the right proof for the shared mock library: it has almost no
 * arithmetic of its own. Everything it does - measuring, copying, joining,
 * upper-casing - it delegates to the PS1 BIOS string vectors (the thunks in
 * src/game/8008CCxx.c). A test of this unit therefore cannot say anything at all
 * unless the BIOS stand-ins answer, and what it can say is exactly what the mock
 * adds over a silent no-op stub:
 *
 *   1. The call *chain*: which vectors, in which order, how many times. A no-op
 *      stub returns 0 from strlen, the unit then writes an empty path, and the
 *      chain is unobservable. The mock answers properly, so the chain itself
 *      becomes the assertion.
 *   2. The end-to-end result, derived from the rules in the unit's own doc block
 *      plus the ASCII meaning of the BIOS services - not recorded from a run.
 *   3. That an unsupported vector is loud: the test script runs the binary a
 *      second time asking for an unimplemented A0 service and expects exit code
 *      2 with the vector named. A mock returning 0 there would let a unit carry
 *      on with a value the BIOS never produces.
 *
 * Every expected value below is written out from src/game/memory-resources/gFormatIso9660Path.c's doc block
 * (separator rules, character filter, the 64-byte refusal, the empty case, the
 * mode-1 ";1" suffix) and counted by hand. None was recorded from the binary.
 */
#include "fc_testkit.h"
#include "mocks/bios_mock.h"

/* The unit, under its retail label. */
s32 gFormatIso9660Path(s32 mode, const u8 *name, const u8 *dir, u8 *out)
    __asm__("func_8001DC48");

/* ---------------------------------------------------------------- helpers */

static u8 out_buf[128];

/* Byte compare that names the first divergence, so a failure says where. */
static void expect_string(const char *what, const char *want)
{
    s32 i = 0;
    while (want[i] && out_buf[i] == (u8)want[i]) i++;
    fck_checks++;
    if ((u8)want[i] != out_buf[i]) {
        fck_write("FAIL: ");
        fck_write(what);
        fck_write(": got \"");
        fck_write((const char *)out_buf);
        fck_write("\" want \"");
        fck_write(want);
        fck_write("\"\n");
        fck_failures++;
    }
}

/* 0xEE fill: any byte the unit was not supposed to write still shows up. */
static void clear_out(void)
{
    s32 i;
    for (i = 0; i < (s32)sizeof(out_buf); i++) out_buf[i] = 0xEE;
}

/* Compare the first `count` recorded services. Total call count is a separate,
 * explicit check - a prefix match must not hide extra calls. */
static void expect_service_prefix(const char *what, const s32 *want, s32 count)
{
    s32 i;
    fck_checks++;
    if (fcm_bios.calls < count) {
        fck_write("FAIL: ");
        fck_write(what);
        fck_write(": only ");
        fck_write_int(fcm_bios.calls);
        fck_write(" BIOS calls, expected at least ");
        fck_write_int(count);
        fck_write("\n");
        fck_failures++;
        return;
    }
    for (i = 0; i < count; i++) {
        if (fcm_bios.services[i] != want[i]) {
            fck_write("FAIL: ");
            fck_write(what);
            fck_write(": step ");
            fck_write_int(i);
            fck_write(" is service ");
            fck_write_int(fcm_bios.services[i]);
            fck_write(", expected ");
            fck_write_int(want[i]);
            fck_write("\n");
            fck_failures++;
            return;
        }
    }
}

/* --- 1. the plain join, mode 0 ------------------------------------------- */

/* name "LEVEL1.DAT" (10), dir "DATA" (4): strlen(name), strlen(dir), strcpy of
 * the directory, one separator appended, strcat of the name, then strlen of the
 * result. The walk then upper-cases all 15 characters plus the terminator, so
 * 5 + 16 = 21 BIOS calls in total. */
static void test_join_with_directory(void)
{
    static const s32 chain[] = {
        FCM_BIOS_A0_STRLEN,   /* strlen(name)          */
        FCM_BIOS_A0_STRLEN,   /* strlen(dir)           */
        FCM_BIOS_A0_STRCPY,   /* buf = dir             */
        FCM_BIOS_A0_STRCAT,   /* buf += name           */
        FCM_BIOS_A0_STRLEN    /* length of the join    */
    };
    s32 r;

    fcm_bios_reset();
    clear_out();
    r = gFormatIso9660Path(0, (const u8 *)"LEVEL1.DAT", (const u8 *)"DATA", out_buf);

    expect_string("mode 0 joins dir and name with a separator", "DATA\\LEVEL1.DAT");
    expect_service_prefix("mode 0 chain", chain, 5);
    FCK_EQ(fcm_bios.calls, 5 + 16, "one toupper per character, terminator included");
    FCK_EQ(fcm_bios.services[5], FCM_BIOS_A0_TOUPPER, "the walk upper-cases");
    FCK_EQ(fcm_bios.last_service, FCM_BIOS_A0_TOUPPER, "the walk ends on toupper");
    FCK_EQ(r, 15, "the return value is the produced path's length");

    /* The directory must be measured before the name is appended, otherwise the
     * separator decision would read the wrong byte. */
    FCK_EQ(fcm_bios.seq_overflow, 0, "the service log was large enough");
}

/* --- 2. a directory that already ends in a separator --------------------- */

/* "DATA\" + "LEVEL1.DAT" must not gain a second backslash: the separator is
 * appended only when neither side of the junction is already one. */
static void test_no_double_separator(void)
{
    fcm_bios_reset();
    clear_out();
    gFormatIso9660Path(0, (const u8 *)"LEVEL1.DAT", (const u8 *)"DATA\\", out_buf);
    expect_string("a trailing separator suppresses the added one", "DATA\\LEVEL1.DAT");
}

/* --- 3. mode 2 with a one-character name uses ':' ------------------------ */

/* The doc block states that mode 2 with a one-character name writes ':' rather
 * than the backslash. "DATA" + "X" is therefore "DATA:X", not "DATA\X". */
static void test_mode2_colon_separator(void)
{
    fcm_bios_reset();
    clear_out();
    gFormatIso9660Path(2, (const u8 *)"X", (const u8 *)"DATA", out_buf);
    expect_string("mode 2 with a one-character name joins with ':'", "DATA:X");
}

/* --- 4. the character filter and the upper-case pass -------------------- */

/* Mode 0 keeps only allowed characters: the space (0x20) and '=' (0x3D) are
 * dropped, the dot survives, letters are upper-cased, so "a b=c.dat" becomes
 * "ABC.DAT". Calls: strlen(name), strcat, strlen(join), then one toupper per
 * character (9 + terminator). */
static void test_character_filter(void)
{
    static const s32 chain[] = {
        FCM_BIOS_A0_STRLEN,   /* strlen(name)       */
        FCM_BIOS_A0_STRCAT,   /* no directory: join */
        FCM_BIOS_A0_STRLEN    /* length of the join */
    };
    s32 r;

    fcm_bios_reset();
    clear_out();
    r = gFormatIso9660Path(0, (const u8 *)"a b=c.dat", (const u8 *)0, out_buf);

    expect_string("illegal characters are dropped and letters upper-cased", "ABC.DAT");
    expect_service_prefix("filter chain", chain, 3);
    FCK_EQ(fcm_bios.calls, 3 + 10, "one toupper per character of the name");
    FCK_EQ(r, 7, "the filtered path is seven characters");
}

/* --- 5. mode 1, the path the retail caller uses ------------------------- */

/* func_8001D424 passes mode 1, so this is the branch that actually runs in the
 * game: the name is upper-cased, the dot is remembered as the extension
 * position, and at the terminator a missing ";1" version suffix is appended.
 * "level1.dat" -> "LEVEL1.DAT;1". */
static void test_mode1_version_suffix(void)
{
    fcm_bios_reset();
    clear_out();
    gFormatIso9660Path(1, (const u8 *)"level1.dat", (const u8 *)0, out_buf);
    expect_string("mode 1 appends the ;1 version suffix", "LEVEL1.DAT;1");
}

/* A name that already carries the version is not given a second one. */
static void test_mode1_existing_version(void)
{
    fcm_bios_reset();
    clear_out();
    gFormatIso9660Path(1, (const u8 *)"level1.dat;1", (const u8 *)0, out_buf);
    expect_string("mode 1 keeps an existing version suffix", "LEVEL1.DAT;1");
}

/* --- 6. nothing to format ----------------------------------------------- */

/* A null name and a null directory return before the first BIOS call: out[0] is
 * zeroed and the function returns 0. No service may be reached at all. */
static void test_empty_input(void)
{
    s32 r;

    fcm_bios_reset();
    clear_out();
    r = gFormatIso9660Path(0, (const u8 *)0, (const u8 *)0, out_buf);

    FCK_EQ(r, 0, "an empty request returns 0");
    FCK_EQ(out_buf[0], 0, "an empty request still terminates the output");
    FCK_EQ(fcm_bios.calls, 0, "an empty request touches no BIOS service");
}

/* --- 7. the 64-byte refusal -------------------------------------------- */

/* pos + namelen + 1 >= 64 is refused with -18 before anything is copied, so only
 * the initial strlen may reach the BIOS and the output stays untouched. */
static void test_length_refusal(void)
{
    u8 name[64];
    s32 i, r;

    for (i = 0; i < 63; i++) name[i] = (u8)'A';
    name[63] = 0;

    fcm_bios_reset();
    clear_out();
    r = gFormatIso9660Path(0, name, (const u8 *)0, out_buf);

    FCK_EQ(r, -18, "a 63-character name is refused");
    FCK_EQ(fcm_bios.calls, 1, "the refusal happens after the measurement only");
    FCK_EQ(fcm_bios.services[0], FCM_BIOS_A0_STRLEN, "only strlen ran");
    FCK_EQ(out_buf[0], 0xEE, "a refused name leaves the output untouched");
}

/* --- 8. the loud-failure contract --------------------------------------- */

/* Reached from the test script's second run. An A0 service this mock does not
 * implement must abort with the vector named, never return 0. */
static int expect_abort_run(void)
{
    fck_write("iso9660-path: probing an unimplemented BIOS service\n");
    fcm_bios_dispatch(0xA0, 0x99, 0, 0, 0);
    fck_write("FAIL: the unimplemented service returned instead of aborting\n");
    return 1;
}

int main(int argc, char **argv)
{
    if (argc > 1 && argv[1][0] == 'a') return expect_abort_run();

    test_join_with_directory();
    test_no_double_separator();
    test_mode2_colon_separator();
    test_character_filter();
    test_mode1_version_suffix();
    test_mode1_existing_version();
    test_empty_input();
    test_length_refusal();

    return fck_report("iso9660-path");
}
