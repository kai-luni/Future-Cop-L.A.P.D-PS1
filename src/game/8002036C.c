#include "game/types.h"

typedef struct Unk8009AB08 {
    u8 unk00[0x8C8];
    s32 unk8C8;
    u8 unk8CC[8];
    s32 unk8D4;
    s32 unk8D8;
    s32 unk8DC;
    s32 unk8E0;
    s32 unk8E4;
    s32 unk8E8;
    s32 unk8EC;
    s32 unk8F0;
} Unk8009AB08;

extern Unk8009AB08 *D_8009AB08;

extern void func_8008C9C0(void);
extern void func_8008C990(s32 arg0);
extern void func_8008C970(s32 arg0);
extern void func_8008C9E0(void);

void func_8002036C(void)
{
    if (--D_8009AB08->unk8C8 == 0) {
        func_8008C9C0();
        func_8008C990(D_8009AB08->unk8D4);
        func_8008C990(D_8009AB08->unk8D8);
        func_8008C990(D_8009AB08->unk8DC);
        func_8008C990(D_8009AB08->unk8E0);
        func_8008C990(D_8009AB08->unk8E4);
        func_8008C990(D_8009AB08->unk8E8);
        func_8008C990(D_8009AB08->unk8EC);
        func_8008C990(D_8009AB08->unk8F0);
        func_8008C970(D_8009AB08->unk8D4);
        func_8008C970(D_8009AB08->unk8D8);
        func_8008C970(D_8009AB08->unk8DC);
        func_8008C970(D_8009AB08->unk8E0);
        func_8008C970(D_8009AB08->unk8E4);
        func_8008C970(D_8009AB08->unk8E8);
        func_8008C970(D_8009AB08->unk8EC);
        func_8008C970(D_8009AB08->unk8F0);
        func_8008C9E0();
    }
}
