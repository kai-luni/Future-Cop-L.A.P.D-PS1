#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 pad[0x04];
};

extern u8 D_8009F3C0;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F458;
extern u8 D_8009BBF8[];
extern struct Entry D_8009B4E4[];
extern s8 D_8009B72C[];

void func_8007A168(void)
{
    if (D_8009F3C0 != 0 || D_8009F3C3 != 0 || D_8009F458 == 8) {
        D_8009BBF8[0x46] = 1;
        *(u16 *)(D_8009BBF8 + 0x4E) = 0x526C;
        D_8009F3C3 = 1;
    }
    if (D_8009F3C0 != 0 || D_8009F3C4 != 0 ||
        (D_8009B4E4[D_8009B72C[0]].unk1F == 0xA &&
         D_8009B4E4[D_8009B72C[1]].unk1F == D_8009B4E4[D_8009B72C[0]].unk1F &&
         D_8009B4E4[D_8009B72C[2]].unk1F == D_8009B4E4[D_8009B72C[1]].unk1F &&
         D_8009B4E4[D_8009B72C[3]].unk1F == D_8009B4E4[D_8009B72C[2]].unk1F &&
         D_8009B4E4[D_8009B72C[4]].unk1F == D_8009B4E4[D_8009B72C[3]].unk1F)) {
        D_8009BBF8[0x9A] = 1;
        *(u16 *)(D_8009BBF8 + 0xA2) = 0x5276;
        D_8009F3C4 = 1;
    }
}
