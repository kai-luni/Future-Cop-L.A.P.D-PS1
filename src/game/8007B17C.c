// Categories: Game Engine > Front-End > Screen Prompt | Graphics > HUD > Prompt Queue | Input > Controller
#include "game/types.h"

struct Unk8009FA94 {
    u8 pad00[0x0C];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
};

struct Unk8009F468 {
    s8 unk0;
    s8 unk1;
    u8 pad2[3];
    s8 unk5;
};

extern struct Unk8009F468 *D_8009F468;
extern struct Unk8009FA94 *gGameSession __asm__("D_8009FA94");
extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern volatile s32 gPaletteCycleCounter __asm__("D_8009FA60");

extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                          s32 alignX, s32 alignY, s32 font);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Queues the front-end prompts of one player column: an
 *        optional header line plus one or two text ids pushed into the
 *        display slots, selected by the current menu record.
 *
 * @param arg0 Player column. 0 also draws the header string and
 *        selects menu row +0x00 and the upper display slot; 1 selects
 *        row +0x01 and the lower slot.
 * @return Nothing.
 *
 * @par Confirmed
 * arg0 == 0 first draws one line with func_800788F0(0, 0xA0, y, 0x10,
 * id, 4, 8, 1): y is 0x37 when D_8009FA94->unk0C == 2 and 0x41
 * otherwise, id is 0x438 when D_8009FA94->unk5C == 1 and 0x42E
 * otherwise. The trailing 4 and 8 are the horizontal and vertical
 * alignment (centre, top) and 1 is the font slot.
 * Selection: when D_8009F468->unk5 == 2 (a two-player screen), arg0 ==
 * 1 takes slot b = 2 and maps menu row +0x01 (5, 6, 4) to variant a
 * (1, 2, 3), while arg0 == 0 takes b = 1 and maps row +0x00 (2, 3, 1).
 * Otherwise the single-player mapping of +0x00 (1, 2, 0) to (1, 2, 3)
 * is used with b = 1.
 * Emission: variant 1 queues 0x6C2 (frame bit 9), 0x6E5 (frame bit 10) or
 * 0x6D6 as slot 0 and 0x6E0 as slot 1 while D_8009F8C0[arg0] is
 * non-zero, and 0x6EA plus 0x44C when it is zero. Variant 2 queues
 * 0x6B8 and 0x44C, variant 3 queues 0x6AE and 0x44C. All of them go
 * through func_80078B34(id, slot, b), which stores the pair selected
 * by b in its record: 0xA0 for b = 0, 0x55 for b = 1 and 0xEB for b = 2.
 * A last func_80078B34(0x10CC, 1, b) runs when frame bit 9 is set.
 *
 * @par Likely role
 * The per-frame prompt refresh of a front-end screen. D_8009F468 is the
 * menu record installed by the front end, its +0x00 and +0x01 the two
 * players' selected rows and +0x05 the screen mode, D_8009F8C0 a
 * per-player "joined/available" byte and D_8009FA60 the frame counter,
 * which gains 5 per frame so bits 9 and 10 alternate roughly every
 * hundred frames. b selects the vertical band, so a two-player screen
 * shows one button prompt at the top and one at the bottom while the
 * single-player screen keeps the upper band.
 *
 * @par Unknown
 * What the text ids mean (only 0x44C is reused by every variant), what
 * D_8009FA94->unk0C and +0x5C distinguish in the header, and which of
 * the two installed screens passes 1.
 *
 * @par Callers
 * No direct callers in the call graph. The address is installed as the
 * +0x18 handler of two front-end screens in the widget factory
 * func_80077AD4: a kind-3 screen whose own slot byte at +0x05 is 1 and a
 * kind-7 screen whose slot byte is 2. analysis/edges.json records it as
 * an "address_taken" edge from 0x80077BCC.
 */
void func_8007B17C(s32 arg0)
{
    s32 b = 0;
    s32 a = 0;

    if (arg0 == 0) {
        struct Unk8009FA94 *g = gGameSession;
        s32 y = (g->unk0C == 2) ? 0x37 : 0x41;
        s32 id = (g->unk5C == 1) ? 0x438 : 0x42E;
        func_800788F0(0, 0xA0, y, 0x10, id, 4, 8, 1);
    }

    if (D_8009F468->unk5 == 2) {
        if (arg0 == 1) {
            b = 2;
            switch (D_8009F468->unk1) {
            case 5: a = 1; break;
            case 6: a = 2; break;
            case 4: a = 3; break;
            }
        } else {
            b = 1;
            switch (D_8009F468->unk0) {
            case 2: a = 1; break;
            case 3: a = 2; break;
            case 1: a = 3; break;
            }
        }
    } else {
        switch (D_8009F468->unk0) {
        case 1: a = 1; break;
        case 2: a = 2; break;
        case 0: a = 3; break;
        }
    }

    switch (a) {
    case 1:
        __asm__ volatile("");
        if (gPlayerMenuDepth[arg0] != 0) {
            s32 id;
            if ((gPaletteCycleCounter >> 9) & 1) {
                id = 0x6C2;
            } else if ((gPaletteCycleCounter >> 10) & 1) {
                id = 0x6E5;
            } else {
                id = 0x6D6;
            }
            func_80078B34(id, 0, b);
            func_80078B34(0x6E0, 1, b);
        } else {
            func_80078B34(0x6EA, 0, b);
            func_80078B34(0x44C, 1, b);
        }
        break;
    case 2:
        func_80078B34(0x6B8, 0, b);
        func_80078B34(0x44C, 1, b);
        break;
    case 3:
        func_80078B34(0x6AE, 0, b);
        func_80078B34(0x44C, 1, b);
        break;
    }

    if ((gPaletteCycleCounter >> 9) & 1) {
        func_80078B34(0x10CC, 1, b);
    }
}
