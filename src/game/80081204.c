#include "game/types.h"

typedef struct SaveSlot {
    u8 title[0x18];
    u8 name1[0x0c];
    u8 name2[0x0c];
    u8 level;
    u8 player1;
    u8 player2;
    u8 player3;
    u8 value28;
    u8 value34;
    u8 value2c;
    u8 value38;
    u8 value30;
    u8 value3c;
    u8 value08;
    u8 value98;
    u8 upgrades[4];
    u8 mode;
    u8 global3c0;
    u8 global3e6;
    u8 global3e7;
    u8 global3c1;
    u8 global3c3;
    u8 upgrade4;
    u8 global3c4;
    u8 value0c;
    u8 pad49[3];
    u8 extra[0x24];
} SaveSlot;

typedef struct SaveMenu {
    u8 pad000[0x200];
    u8 state;
    u8 enabled;
    u8 enabled2;
    u8 pad203;
    u8 unk204;
    u8 pad205[0x71c];
    u8 mode;
    u8 unk922;
    u8 pad923;
    u8 selected;
    u8 text[0x1d];
    u16 timer;
} SaveMenu;

typedef struct GameState {
    u8 pad00[8];
    s32 value08, value0c;
    u8 pad10[0x18];
    s32 value28, value2c, value30, value34, value38, value3c;
    u8 pad40[0x1c];
    s32 mode;
    u8 pad60[0x2c];
    s32 player1, player2, player3, value98;
} GameState;

typedef struct Upgrade {
    u8 pad00[0x1f];
    u8 level;
    u8 current;
    u8 pad21[3];
} Upgrade;

typedef struct FontEntry {
    u8 pad00[6];
    s16 height;
} FontEntry;

extern SaveMenu *D_8009F92C;
extern GameState *D_8009FA94;
extern Upgrade D_8009B4E4[];
extern s8 D_8009B72C[];
extern s8 D_8009F8C0[];
extern s32 D_8009F8F8[];
extern u8 D_800A3928[];
extern u8 D_800A6FE0[];
extern u8 D_800A3718[];
extern u8 *D_8009F878;
extern s16 D_8009F87C;
extern s32 D_8009F8B8;
extern u8 D_8009F8BD, D_8009F8BE;
extern s8 D_8009F880, D_8009F881, D_8009F882;
extern u8 D_8009F3C0, D_8009F3C1, D_8009F3C3, D_8009F3C4;
extern u8 D_8009F3E6, D_8009F3E7, D_8009F3F1, D_8009F458, D_8009F459;
extern u8 D_8009FA10;
extern s8 *D_8009F468;
extern u16 D_8009F470;
extern u8 D_8009F484[], D_8009F498[], D_8009F4A0[], D_8009F4A4[];
extern u8 D_800120BC[], D_800120C8[], D_800120D4[];
extern u8 D_8009F3F8[];
extern s32 D_8009FA60[];

extern void func_8007AA9C(u8 *, u8 *, s32, u8 *);
extern void func_8007A9D0(u8 *, u8 *, s32, u8 *, s32);
extern void func_80080984(void *, s32);
extern void func_8008CCB0(void *, void *);
extern s32 func_8008CCC0(void *);
extern void func_8008CD30(void *, void *, ...);
extern u8 *func_80089914(s32);
extern void func_80078650(void *, s32);
extern s32 func_80046C84(s32, s32);
extern void func_80077FFC(void);
extern void func_80077708(void);
extern void func_800776B8(void);
extern void func_8007FACC(void);
extern void func_80078210(void);
extern void func_80080B70(void);
extern void func_800492A0(u8 *, s32 *, s32 *);
extern FontEntry *func_80048E88(void);
extern void func_80082EF0(s32, s32, s32, s32, u32, s32, s32, s32);
extern void func_80049100(u8 *, s32, s32, s32);
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);

#define SLOT(i) (((SaveSlot *)((u8 *)D_8009F92C + 0x428))[i])
#define SELECTED SLOT(D_8009F92C->selected)

extern u8 D_800120A4[];
extern s32 func_80020D3C(void *, s32, u32);

void func_80081204(void)
{
    ((u8 *)D_8009F92C)[0x922] = 3;
    ((u8 *)D_8009F92C)[0x201] = 1;
    ((u8 *)D_8009F92C)[0x202] = 1;
    D_8009F8BD = 0;
    D_8009F8BE = 0;
    func_80020D3C(D_800120A4, (s32)((u8 *)D_8009F92C + 0x20C), 0x678);
}
