#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad[3];
};

extern u8 D_8009F3C0;
extern u8 D_8009F3C2;
extern u8 D_8009F880;
extern u8 D_8009F881;
extern u8 D_8009F882;
extern s8 *D_8009F884;
extern u8 D_8009F888;
extern u8 D_8009F889;
extern struct Entry D_8009B4E4[];
extern s8 D_8009B72C[];

void func_80077FFC(void);

void func_80077708(void)
{
    D_8009F880 = 0;
    D_8009F881 = 0;
    D_8009F889 = 1;

    if (D_8009F3C0 != 0) {
        D_8009B4E4[D_8009B72C[0]].unk1F = 10;
        D_8009B4E4[D_8009B72C[1]].unk1F = 10;
        D_8009B4E4[D_8009B72C[2]].unk1F = 10;
        D_8009B4E4[D_8009B72C[3]].unk1F = 10;
        D_8009B4E4[D_8009B72C[0]].unk20 = 10;
        D_8009B4E4[D_8009B72C[1]].unk20 = 10;
        D_8009B4E4[D_8009B72C[2]].unk20 = 10;
        D_8009B4E4[D_8009B72C[3]].unk20 = 10;
        D_8009B4E4[D_8009B72C[4]].unk1F = 10;
        D_8009B4E4[D_8009B72C[4]].unk20 = 10;
        func_80077FFC();
    }

    D_8009F884 = D_8009B72C;
    D_8009F882 = D_8009F3C2 + 3;
    D_8009F888 = D_8009F3C2 + 4;
}
