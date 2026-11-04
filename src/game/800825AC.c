#include "game/types.h"

typedef struct Unk824CCHeader {
    u16 type;
    u16 count;
    u32 magic;
} Unk824CCHeader;

typedef struct Unk824CCObject {
    Unk824CCHeader *header;
} Unk824CCObject;

extern Unk824CCObject *D_8009F460;
extern Unk824CCHeader *D_8009F45C;

extern void func_800181D4(Unk824CCObject *object);

void func_800825AC(Unk824CCObject *object)
{
    register s32 flag __asm__("$16") = 0;
    Unk824CCHeader *header;

    if (D_8009F460 != 0) {
        func_800181D4(D_8009F460);
    }
    header = object->header;
    D_8009F460 = object;
    D_8009F45C = header;
    if (header->type != 2 || header->magic != 0x35BFBC73) {
        flag = 1;
    }
    if (flag != 0) {
        func_800181D4(D_8009F460);
        D_8009F460 = 0;
        D_8009F45C = 0;
    }
}
