#include "game/types.h"

typedef struct {
    u8 pad00[0x7B74];
    s32 unk7B74;
    s32 unk7B78;
    s32 unk7B7C;
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;

__asm__(
".text\n"
".globl func_800867B0\n"
"func_800867B0:\n"
"lw $2,0x2C4($28)\n"
);
