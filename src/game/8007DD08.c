// Categories: Game Engine > Front-End Configuration | Graphics > HUD > Loadout | Graphics > 2D Drawing > Text
typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

struct Entry {
    unsigned int unknown;
    u8 *values;
    u8 unused;
    u8 replacement;
    u8 limit;
    u8 pad[9];
};
extern struct Entry gLoadoutTable[] __asm__("D_8009C0A8");
extern s8 *gFontBankTable __asm__("D_8009F468");
extern u8 gPlayerNameTable[] __asm__("D_800A6FE0");
extern s8 D_8009F924[];
extern s8 D_8009F926[];
extern s8 D_8009F928[];
extern u8 gRowRecordTable[] __asm__("D_8009C030");
extern u8 D_8009C050[];
extern u8 *D_8009C0D0;
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");

extern void gClampLoadoutSelections(s32) __asm__("func_8007D2DC");
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);
extern void gDrawLoadoutListRow(u8 *, s32, s32, s32, s32, s32) __asm__("func_8007D7CC");

/**
 * @brief Draws the loadout screen for one player: names, three equipment
 *        rows and the prompt/over-limit display requests.
 *
 * @param arg0 Player index (0/1).
 * @return Nothing.
 *
 * @par Confirmed
 * func_8007D2DC(0) runs first. In the two-player layout (D_8009F468[5] ==
 * 2) both names (D_800A6FE0 and +9) are drawn at x 0x23/0xA4 with a 10000
 * score line below, and the three rows via func_8007D7CC at y 0x4F/0x71/
 * 0x93; otherwise the single name is centred at 0x53 and the rows sit at
 * x 0x86, y 0x35/0x5F/0x89. The row records come from D_8009C030/
 * D_8009C050/D_8009C0D0 indexed by D_8009F924/926/928[arg0] (6-byte
 * stride); the highlighted row is s4 = D_8009F468[arg0] - 1 (player 1:
 * -5). The prompt state s6 is 1 when the mode byte is 0, 2 when the row's
 * replacement != limit. Display requests: 0x2968 (slot 0), 0x521C when
 * values[arg0] > limit (the over-limit marker), 0x10CC (slot 1), and with
 * palette bit 9 set 0x297C (s6 == 1) or 0x2972 (s6 == 2).
 *
 * @par Likely role
 * The equipment selection screen: the three rows are the weapon/upgrade
 * slots drawn by func_8007D7CC, the 0x521C request is the highlighted
 * over-capacity warning the status pass func_80078B98 colours specially.
 *
 * @par Unknown
 * The labels behind the display ids 0x2968/0x2972/0x297C.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 stores the address
 * in two screen callback blocks (D_800A3628.unk18 and D_800A3658.unk18),
 * so the screen dispatcher reaches it indirectly.
 */
void gDrawLoadoutScreen(s32 arg0) __asm__("func_8007DD08");

void gDrawLoadoutScreen(s32 arg0)
{
    s32 s3 = 0;
    s32 s6 = 0;
    s32 s5;
    s32 s0;
    s32 s4;
    s32 fp = gFontBankTable[arg0];
    s32 i3;

    gClampLoadoutSelections(s3);
    s5 = gFontBankTable[5];
    s4 = fp - 1;
    if (s5 == 2) {
        func_800788F0(gPlayerNameTable, 0x23, 0x35, 0x10, 0, 1, 8, s5);
        func_800788F0((u8 *)s3, 0x23, 0x3F, 0x10, 10000, 1, 8, s5);
        func_800788F0(gPlayerNameTable + 9, 0xA4, 0x35, 0x10, 0, 1, 8, s5);
        func_800788F0((u8 *)s6, 0xA4, 0x3F, 0x10, 10000, 1, 8, s5);
        s0 = 0xA4;
        if (arg0 != 0) {
            s4 = fp - 5;
            s3 = 2;
            if (gFontBankTable[arg0] == 4)
                s6 = 1;
            else if (gLoadoutTable[s4].replacement != gLoadoutTable[s4].limit)
                s6 = 2;
        } else {
            s0 = 0x25;
            s3 = 1;
            if (gFontBankTable[0] == 0)
                s6 = 1;
            else if (gLoadoutTable[s4].replacement != gLoadoutTable[s4].limit)
                s6 = 2;
        }
        gDrawLoadoutListRow(gRowRecordTable + D_8009F924[arg0] * 6, s0, 0x4F, s4 == 0, 0, 0);
        gDrawLoadoutListRow(D_8009C050 + D_8009F926[arg0] * 6, s0, 0x71, s4 == 1, 1, 0);
        i3 = D_8009F928[arg0];
        gDrawLoadoutListRow(D_8009C0D0 + i3 * 6, s0, 0x93, s4 == 2, 2, 0);
    } else {
        if (gFontBankTable[arg0] == 0)
            s6 = 1;
        else if (gLoadoutTable[s4].replacement != gLoadoutTable[s4].limit)
            s6 = 2;
        func_800788F0(gPlayerNameTable, 0x53, 0x3C, 0x10, 0, 4, 8, 2);
        func_800788F0(0, 0x53, 0x44, 0x10, 10000, 4, 8, 2);
        gDrawLoadoutListRow(gRowRecordTable + D_8009F924[arg0] * 6, 0x86, 0x35, s4 == 0, 0, 1);
        gDrawLoadoutListRow(D_8009C050 + D_8009F926[arg0] * 6, 0x86, 0x5F, s4 == 1, 1, 1);
        i3 = D_8009F928[arg0];
        gDrawLoadoutListRow(D_8009C0D0 + i3 * 6, 0x86, 0x89, s4 == 2, 2, 1);
    }
    func_80078B34(0x2968, 0, s3);
    if (s4 >= 0 && gLoadoutTable[s4].values[arg0] > gLoadoutTable[s4].limit)
        func_80078B34(0x521C, 0, s3);
    func_80078B34(0x10CC, 1, s3);
    if ((gPaletteCycleCounter >> 9) & 1) {
        switch (s6) {
        case 1: func_80078B34(0x297C, 1, s3); break;
        case 2: func_80078B34(0x2972, 1, s3); break;
        }
    }
}
