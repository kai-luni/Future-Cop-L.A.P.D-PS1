#include "game/types.h"

typedef struct {
    u32 unk0;
    u8 pad[0x20];
} UnkA78F0;

extern void (*D_8009F99C)(void);
extern s32 D_8009F9CC;
extern s32 D_8009FA00;
extern volatile s32 D_8009FA2C;
extern volatile u8 D_8009FA3C;
extern s32 D_8009FA60;
extern UnkA78F0 D_800A78F0[];

extern s32 func_8001EF34(void);
extern void func_8001EC94(u32 arg0);
extern s32 func_80092C8C(s32 arg0);
extern s32 func_8001BFE4(void);
extern void func_8001BFA8(void);

u32 func_8001EF8C(u32 arg0)
{
    if (arg0 < 2 && func_8001EF34() != 0) {
        func_8001EC94(arg0);
        return ~D_800A78F0[arg0].unk0;
    }
    return 0;
}
