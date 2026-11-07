#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 pad[4];
};

struct UnkFA94 {
    u8 pad00[0x8C];
    u8 unk8C;
    u8 pad8D[3];
    u8 unk90;
    u8 pad91[3];
    u8 unk94;
};

extern struct Entry D_8009B4E4[];
extern s8 D_8009B72C[];
extern struct UnkFA94 *D_8009FA94;
extern u8 D_8009F3C0;
extern u8 D_8009F3C1;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F458;
extern u8 D_800A3938[];

extern void func_8007E8D8(u8 *output, u32 value1, u32 value2, u32 value3,
                          u32 value4, u32 value5, u32 value6, u32 value7,
                          u32 value8, u32 value9, u32 value10, u32 value11,
                          u32 value12, u32 value13);

void func_8007F02C(void)
{
    func_8007E8D8(D_800A3938,
                  D_8009FA94->unk8C,
                  D_8009FA94->unk90,
                  D_8009FA94->unk94,
                  D_8009F3C0,
                  D_8009F458,
                  D_8009B4E4[D_8009B72C[0]].unk1F,
                  D_8009B4E4[D_8009B72C[1]].unk1F,
                  D_8009B4E4[D_8009B72C[2]].unk1F,
                  D_8009B4E4[D_8009B72C[3]].unk1F,
                  D_8009F3C1,
                  D_8009F3C3,
                  D_8009B4E4[D_8009B72C[4]].unk1F,
                  D_8009F3C4);
}
