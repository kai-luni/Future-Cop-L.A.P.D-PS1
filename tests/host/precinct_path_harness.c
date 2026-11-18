/* Host harness for the single-player Precinct Assault list configuration.
 *
 * Exercises three unchanged retail functions against retail-derived checks:
 *   gSetupCrimeWarMissionList         (func_800776B8)
 *   gSetupPrecinctAssaultMissionList  (func_80077708)
 *   gUpdatePrecinctUnlockFlag         (func_80077FFC)
 *
 * Expected values derive from the retail tables in SLUS_007.39:
 *   D_8009B724 = { 9, 11, 5, 1, 13, 4, 2, 0 }  (Crime War list, 8 entries)
 *   D_8009B72C = { 3, 7, 10, 15, 6 }           (Precinct list, 5 entries)
 * and from the C sources: list setup clears the cursor indices, sets the
 * list mode, binds the table pointer and entry count; the unlock flag
 * mirrors the state bytes of entries 0..3.
 */
typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern void gSetupCrimeWarMissionList(void) __asm__("func_800776B8");
extern void gSetupPrecinctAssaultMissionList(void) __asm__("func_80077708");
extern void gUpdatePrecinctUnlockFlag(void) __asm__("func_80077FFC");

/* Data image: globals the three units touch, defined here under their
 * retail asm names. */
u8 gFrontEndListFlag __asm__("D_8009F3C0");
u8 gUnlockFlag __asm__("D_8009F3C2");
u8 gSelectedMissionIndex __asm__("D_8009F880");
u8 gMissionListMinIndex __asm__("D_8009F881");
u8 gMissionListMaxIndex __asm__("D_8009F882");
s8 *gMissionListTablePtr __asm__("D_8009F884");
u8 gMissionListEntryCount __asm__("D_8009F888");
u8 gMissionListMode __asm__("D_8009F889");

struct Entry {
    u8 unk00[0x1F];
    u8 state1F;
    u8 state20;
    u8 pad[3];
};
u8 gCrimeWarProgress __asm__("D_8009F458");
u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
struct Entry gWideTextTable[16 * 3] __asm__("D_8009B4E4");
s8 gMissionListTable[8] __asm__("D_8009B724") = { 9, 11, 5, 1, 13, 4, 2, 0 };
s8 gFrontEndListTable[5] __asm__("D_8009B72C") = { 3, 7, 10, 15, 6 };

void __main(void) {}
static long sys3(long n, long a, long b, long c) {
    register long v __asm__("$2") = n; register long x __asm__("$4") = a;
    register long y __asm__("$5") = b; register long z __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v) : "r"(x), "r"(y), "r"(z)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
    return v;
}
static void say(const char *s) { int n = 0; while (s[n]) n++; sys3(4004, 1, (long)s, n); }

static int fails;

static void check(const char *what, long got, long want) {
    if (got != want) {
        say("FAIL "); say(what); say("\n");
        fails++;
    }
}

int main(void) {
    /* Case 1: Crime War list setup, cheat flag clear. */
    gFrontEndListFlag = 0;
    gSetupCrimeWarMissionList();
    check("cw-min", gSelectedMissionIndex, 0);
    check("cw-lower", gMissionListMinIndex, 0);
    check("cw-mode", gMissionListMode, 0);
    check("cw-max", gMissionListMaxIndex, 0);
    check("cw-count", gMissionListEntryCount, 8);
    if (gMissionListTablePtr != gMissionListTable) {
        say("FAIL cw-ptr\n"); fails++;
    }

    /* Case 2: Precinct list setup, cheat flag clear (max limited to 3, count 4). */
    gFrontEndListFlag = 0;
    gSetupPrecinctAssaultMissionList();
    check("pa-min", gSelectedMissionIndex, 0);
    check("pa-lower", gMissionListMinIndex, 0);
    check("pa-mode", gMissionListMode, 1);
    check("pa-max", gMissionListMaxIndex, gUnlockFlag + 3);
    check("pa-count", gMissionListEntryCount, gUnlockFlag + 4);
    if (gMissionListTablePtr != gFrontEndListTable) {
        say("FAIL pa-ptr\n"); fails++;
    }

    /* Case 3: unlock flag mirrors the entry states (all four equal to 10 -> 1). */
    gWideTextTable[3].state1F = 10;
    gWideTextTable[7].state1F = 10;
    gWideTextTable[10].state1F = 10;
    gWideTextTable[15].state1F = 10;
    gUnlockFlag = 0;
    gUpdatePrecinctUnlockFlag();
    check("unlock-all", gUnlockFlag, 1);

    /* Case 4: one differing state clears the flag. */
    gWideTextTable[15].state1F = 9;
    gUpdatePrecinctUnlockFlag();
    check("unlock-clear", gUnlockFlag, 0);

    /* Case 5: Precinct setup with cheat flag set unlocks all five entries. */
    gFrontEndListFlag = 1;
    gSetupPrecinctAssaultMissionList();
    check("cheat-state0", gWideTextTable[3].state1F, 10);
    check("cheat-state4", gWideTextTable[6].state1F, 10);
    check("cheat-count", gMissionListEntryCount, gUnlockFlag + 4);

    if (fails) {
        say("precinct-path: FAILURES\n");
        return 1;
    }
    say("precinct-path: ALL PASS\n");
    return 0;
}
