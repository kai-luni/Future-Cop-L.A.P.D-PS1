#include "game/types.h"

struct LinkState;
struct LinkNode;

struct Unk49C38Entry {
    u8 pad00[7];
    u8 id;
};

struct Unk49C38List {
    volatile s32 count;
    struct Unk49C38Entry entries[1];
};

struct Unk49C38Node {
    s16 unk00;
    s16 unk02;
    s32 unk04;
    u8 unk08;
    u8 unk09;
    u8 unk0A;
    u8 unk0B;
    s32 unk0C;
    u16 unk10;
};

struct Unk49C38 {
    u8 pad00[8];
    u32 unk08;
    u32 unk0C;
    u32 unk10;
    u8 unk14;
    s8 unk15;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
    s16 unk20;
    s8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    u16 unk26;
    u16 unk28;
};

extern struct Unk49C38List *D_8009F778;
extern s32 D_8009F780;
extern struct LinkState *D_8009F784;
extern s32 D_8009FAA4[];

extern struct LinkNode *func_800195A4(struct LinkState *state);
extern s32 func_80019668(void);
extern void func_800497A8(void);
extern void func_80049980(struct Unk49C38Node *node, u32 arg1, u32 arg2, s32 arg3,
                          s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8);
extern s32 func_80067684(u8 *arg0, s32 arg1);

s32 func_80049C38(struct Unk49C38 *object, s32 arg1)
{
    struct Unk49C38Node *node;
    register s32 index __asm__("$17");
    register s32 i __asm__("$4");
    register s32 key __asm__("$3");
    s32 value;
    s32 callArg;
    s32 var_s2;
    s32 var_s1;
    s32 var_v1;
    s32 var_s4;

    if (object->unk18 < 0x40) {
        if (object->unk1A < 0x40) {
            return 0;
        }
    }
    if (D_8009F780 == 0) {
        return 0;
    }
    key = object->unk15;
    index = key;
    if (key >= 0) {
        register struct Unk49C38List *list __asm__("$5");
        register struct Unk49C38List *q __asm__("$3");
        s32 key2;
        list = D_8009F778;
        i = 0;
        if (i >= list->count) goto done;
        key2 = key;
    loop:
        if (list->entries[i].id == key2) goto done;
        q = list;
        i++;
        if (i < q->count) goto loop;
    done:
        index = i;
    }
    node = (struct Unk49C38Node *)func_800195A4(D_8009F784);
    if (node == 0) {
        func_800497A8();
        node = (struct Unk49C38Node *)func_800195A4(D_8009F784);
        if (node == 0) {
            return 0;
        }
    }
    node->unk00 = index;
    if (index < 0) {
        node->unk0C = D_8009FAA4[0];
        node->unk10 = object->unk28;
    }
    node->unk09 = object->unk23;
    node->unk0A = object->unk24;
    node->unk0B = object->unk25;
    node->unk08 = object->unk14;
    if (arg1 >= 0) {
        arg1 += D_8009FAA4[0];
    }
    node->unk04 = arg1;
    var_s4 = 0;
    value = object->unk22;
    if (value != -3) {
        if (value != 0) {
            if (value == 1) {
                callArg = -2;
            } else {
                callArg = value - 2;
            }
        } else {
            callArg = -1;
        }
        var_s4 = func_80067684((u8 *)&object->unk08, callArg);
    }
    node->unk02 = -1;
    var_s2 = object->unk1C;
    if (var_s2 < 0) {
        var_s2 = func_80019668() & 0xFFF;
    }
    var_s1 = object->unk1E;
    if (var_s1 < 0) {
        var_s1 = func_80019668() & 0xFFF;
    }
    var_v1 = object->unk20;
    if (var_v1 < 0) {
        var_v1 = func_80019668() & 0xFFF;
    }
    func_80049980(node, object->unk08, object->unk10,
                  var_s4 + (object->unk16 << 4),
                  object->unk18 << 4, object->unk1A << 4,
                  var_s2, var_s1, var_v1);
    return (s32)node;
}
