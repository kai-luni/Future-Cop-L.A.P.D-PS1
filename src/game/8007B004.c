// Categories: Game Engine > State Initialization
#include "game/types.h"

typedef struct Unk7B004Mode {
    u8 unk00[8];
    s32 unk08;
    s32 unk0C;
    u8 unk10[0x4C];
    s32 unk5C;
    u8 unk60[0x38];
    s32 unk98;
} Unk7B004Mode;

typedef struct Unk7B004Node {
    u8 pad00[0x24];
    struct Unk7B004Node *unk24;
} Unk7B004Node;

extern Unk7B004Mode *gGameSession __asm__("D_8009FA94");
extern volatile s32 D_8009F8E0;
extern volatile s32 D_8009F8E4;
extern volatile s32 gPlayerSelection __asm__("D_8009F8F0");
extern s32 D_8009F8F4;
extern Unk7B004Node *gFontBankTable __asm__("D_8009F468");
extern u8 D_8009BB34[];
extern Unk7B004Node D_800A3688;
extern u8 D_800A37AB[];
extern u8 D_800A37DB[];

/**
 *  @brief Initialize shared state from the current mode and adjust a linked node.
 *
 *  Copies two mode fields, clears two counters, and sets two halfwords to 100.
 *  It also selects one of two byte locations and a value from the mode fields.
 *  @par Globals
 *  D_8009FA94 supplies mode fields; D_8009F8F0 and D_8009F8F4 receive copies.
 *  D_8009F8E0 and D_8009F8E4 are cleared; D_8009BB34 holds the halfwords.
 *  D_8009F468 and D_800A3688 identify the link to bypass when present.
 *  D_800A37AB or D_800A37DB receives the selected byte.
 *  @par Control flow
 *  Conditionally bypasses one node, then branches on two mode fields.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes shared counters, halfwords, a possible link, and one selected byte.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8007B004(void)
{
    register Unk7B004Mode *mode __asm__("$3");
    Unk7B004Node *node;
    u8 *base;
    s32 v98;

    mode = gGameSession;
    gPlayerSelection = mode->unk08;
    __asm__ volatile("" ::: "memory");
    base = D_8009BB34;
    __asm__ volatile("" ::: "memory");
    v98 = mode->unk98;
    __asm__ volatile("" ::: "memory");
    D_8009F8E0 = 0;
    D_8009F8E4 = 0;
    *(s16 *)(base + 0x32) = 0x64;
    __asm__ volatile("" ::: "memory");
    *(s16 *)(base + 0x86) = 0x64;
    D_8009F8F4 = v98;
    __asm__ volatile("" ::: "memory");
    node = gFontBankTable;
    if (node->unk24 == &D_800A3688) {
        node->unk24 = node->unk24->unk24;
    }


    mode = gGameSession;
    if (mode->unk5C == 1) {
        if (mode->unk0C == 1) {
            D_800A37AB[0] = 9;
        } else {
            D_800A37DB[0] = 3;
        }
    } else {
        if (mode->unk0C == 1) {
            D_800A37AB[0] = 1;
        } else {
            D_800A37DB[0] = 8;
        }
    }
}
