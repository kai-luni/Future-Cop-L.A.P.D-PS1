#include "game/types.h"

struct State86 {
    char pad[0x7B20];
    s32 active;
    s32 value;
    s32 count;
    char pad2[0x38];
    void *first;
    void *second;
};
struct Setup86 {
    s32 type;
    s16 a;
    s16 b;
    char pad[0x20];
};
struct Msg86 {
    void *object;
    s32 kind;
    s16 a;
    s16 b;
    char pad0[8];
    s16 c;
    char pad1[6];
    s32 value;
    char pad2[4];
    s32 one;
    s32 one2;
    s32 three;
    s16 d;
    s16 e;
    s16 f;
    s16 g;
    s16 h;
    char pad3[6];
};
extern struct State86 *D_8009F4D8;
extern void func_80098FE8(struct Setup86 *);
extern s32 func_800452C8(void);
extern void func_80099368(struct Msg86 *);

void func_80086C74(void *object, s32 value, s32 c)
{
    struct Msg86 msg;
    struct Setup86 setup;

    setup.type = 3;
    setup.a = 0x3FFF;
    setup.b = 0x3FFF;
    func_80098FE8(&setup);
    msg.object = object;
    msg.kind = 0xFF93;
    msg.a = object == D_8009F4D8->first ? func_800452C8() * 127 : 0;
    msg.b = object == D_8009F4D8->second ? func_800452C8() * 127 : 0;
    msg.c = c;
    msg.value = value;
    msg.one = 1;
    msg.one2 = 1;
    msg.three = 3;
    msg.d = 0;
    msg.e = 0;
    msg.f = 0;
    msg.g = 0;
    msg.h = 15;
    func_80099368(&msg);
}

