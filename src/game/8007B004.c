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

extern Unk7B004Mode *D_8009FA94;
extern volatile s32 D_8009F8E0;
extern volatile s32 D_8009F8E4;
extern volatile s32 D_8009F8F0;
extern s32 D_8009F8F4;
extern Unk7B004Node *D_8009F468;
extern u8 D_8009BB34[];
extern Unk7B004Node D_800A3688;
extern u8 D_800A37AB[];
extern u8 D_800A37DB[];

void func_8007B004(void)
{
    register Unk7B004Mode *mode __asm__("$3");
    Unk7B004Node *node;
    u8 *base;
    s32 v98;

    mode = D_8009FA94;
    D_8009F8F0 = mode->unk08;
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
    node = D_8009F468;
    if (node->unk24 == &D_800A3688) {
        node->unk24 = node->unk24->unk24;
    }


    mode = D_8009FA94;
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
