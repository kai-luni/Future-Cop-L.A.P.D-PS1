// Categories: Game Engine > State Transitions | Input > Port Control
#include "game/types.h"

struct Entry78C88 {
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
};

struct Obj78C88 {
    u8 pad00[0x0C];
    s32 unk0C;
    u8 pad10[0x3C];
    s32 unk4C;
    u8 pad50[0x0C];
    s32 unk5C;
};

struct Sub78C88 {
    s32 unk00;
    u8 pad04[8];
    u8 unk0C;
    u8 pad0D[0x17];
};

extern struct Entry78C88 D_8009B730[];
extern struct Sub78C88 D_8009B4E4[];
extern struct Obj78C88 *D_8009FA94;
extern s32 D_8009FA84;
extern s32 D_8009FA00;
extern u8 D_8009F3F2;
extern u8 D_8009F3F0;
extern u8 D_8009F45A;
extern u8 D_8009F8D8;
extern u8 D_8009F8D9;
extern u8 D_8009B300[];

extern void func_8008CCB0(u8 *a0, s32 a1);
extern u8 func_8001EB2C(s32 arg0);
extern void func_8001EC04(s32 port, s32 enable);

/**
 *  @brief Advances a pending entry sequence and applies its selected state.
 *
 *  A nonzero sequence index selects an entry and advances through indices below five.
 *  The selected entry supplies object fields and a subtable value for setup.
 *  @return 1 if an entry was applied, or 0 if the sequence was inactive.
 *  @par Globals
 *  D_8009F3F2 indexes D_8009B730; D_8009FA94 receives entry fields.
 *  D_8009B4E4 and D_8009B300 supply setup data; D_8009FA84, D_8009FA00, D_8009F45A and D_8009F3F0 receive state.
 *  D_8009F8D8 and D_8009F8D9 store the two port query results.
 *  @par Control flow
 *  An inactive index returns immediately; the object field at 0x4C changes only when entry byte 1 is nonzero.
 *  @par Calls
 *  func_8008CCB0 applies setup data; func_8001EB2C queries each port; func_8001EC04 disables both ports.
 *  @par Side effects
 *  Updates sequence, object and global state and issues port-control calls.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80078C88(void)
{
    struct Entry78C88 *e;
    struct Obj78C88 *o;
    u8 unk0C;

    e = 0;
    if (D_8009F3F2 != 0) {
        e = &D_8009B730[D_8009F3F2];
    }
    if (D_8009F3F2 != 0) {
        D_8009F3F2 = D_8009F3F2 + 1;
        if (D_8009F3F2 >= 5) {
            D_8009F3F2 = 0;
        }
    }
    if (e == 0) {
        return 0;
    }
    o = D_8009FA94;
    o->unk0C = e->unk2;
    o->unk5C = e->unk1;
    if (e->unk1 != 0) {
        o->unk4C = e->unk3;
    }
    func_8008CCB0(D_8009B300, D_8009B4E4[e->unk0].unk00);
    unk0C = D_8009B4E4[e->unk0].unk0C;
    D_8009FA84 = 1;
    D_8009FA00 = 5;
    D_8009F45A = 3;
    D_8009F3F0 = unk0C;
    D_8009F8D8 = func_8001EB2C(0);
    D_8009F8D9 = func_8001EB2C(1);
    func_8001EC04(0, 0);
    func_8001EC04(1, 0);
    return 1;
}
