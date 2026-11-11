#include "game/types.h"

struct Unk2D664Obj {
    u8 pad00[0x16];
    u8 unk16;
    u8 pad17[5];
    u16 unk1C;
    u8 pad1E[6];
    u32 flags;
    u8 pad28[0x64];
    u32 unk8C;
    u8 pad90[0x4F];
    u8 unkDF;
    u8 padE0[0x16];
    u8 unkF6;
    u8 unkF7;
    u8 padF8[6];
    u8 unkFE;
    u8 padFF[1];
    struct Unk2D664Obj *unk100;
    struct Unk2D664Obj *unk104;
};

struct Unk2D664AC {
    u8 pad00[0x10];
    u16 unk10;
    u8 pad12[0xAF];
    s8 unkC1;
};

struct Unk2D664Player {
    u8 pad00[0x18];
    u8 unk18;
    u8 pad19[0x93];
    struct Unk2D664AC *unkAC;
};

struct Unk2D664Inner {
    u8 pad00[0x18];
    u16 unk18;
    u8 pad1A[0x1E];
    u16 unk38;
};

struct Unk2D664State {
    u8 pad00[0x14];
    u16 unk14;
    u8 pad16[2];
    u8 unk18;
    u8 pad19[0x11B];
    struct Unk2D664Inner *unk134;
};

struct Unk2D664Menu {
    u8 pad00[0x68];
    s32 unk68;
    u8 pad6C[0x14];
    s32 unk80;
    u8 pad84[0x5C];
    s32 unkE0;
};

struct Unk2D664Entity {
    struct Unk2D664Entity *next;
    void *update;
    s32 id;
    u8 pad0C[8];
    u16 unk14;
};

extern s32 D_8009FA0C;
extern struct Unk2D664State *D_8009F9EC;
extern struct Unk2D664Menu *D_8009FA94;
extern struct Unk2D664Player *D_8009FA58[2];

extern s32 func_8002AD58(s32 message, struct Unk2D664Obj *object, void *data,
                         s32 arg3);
extern void func_8002C584(struct Unk2D664Obj *obj, void *data, s32 arg2);
extern s32 func_8002CC30(struct Unk2D664Obj *obj, void *data);
extern void func_8002D1A0(struct Unk2D664Obj *obj, void *data);
extern void func_80063194(s32 key);
extern struct Unk2D664Entity *func_8004D664(s32 id);

s32 func_8002D664(s32 message, struct Unk2D664Obj *obj, void *arg2, s32 arg3)
{
    struct Unk2D664State *p;
    struct Unk2D664Entity *found;
    register s32 kind __asm__("$8");
    s32 i;
    s32 ret;
    struct Unk2D664Menu *menu;
    struct Unk2D664State *st;
    struct Unk2D664Player **players;

    switch (message) {
    case 1:
        return 0x118;
    case 2:
        func_8002C584(obj, arg2, arg3);
        return 0;
    case 6:
        return func_8002CC30(obj, arg2);
    case 10:
        if (obj->unk16 == 0x24) {
            func_8002D1A0(obj, arg2);
        }
        ret = 0;
        if (obj->unk104 != arg2) {
            ret = obj->unk100 != arg2;
        }
        return ret;
    case 9:
    case 11:
    case 14:
        func_8002AD58(message, obj, arg2, arg3);
        if (obj->unk104 != 0) {
            if (obj->flags & 0x2000000) {
                obj->unk104->flags |= 0x2000000;
            } else {
                obj->unk104->flags &= ~0x2000000;
            }
            obj->unk104->unk1C = obj->unk1C;
        }
        if (obj->unk100 != 0) {
            if (obj->flags & 0x2000000) {
                obj->unk100->flags |= 0x2000000;
            } else {
                obj->unk100->flags &= ~0x2000000;
            }
            obj->unk100->unk1C = obj->unk1C;
        }
        return 0;
    case 4:
        if (obj->unk104 != 0) {
            obj->unk104->flags |= 8;
        }
        if (obj->unk100 != 0) {
            obj->unk100->flags |= 8;
        }
        break;
    case 5:
        if (obj->unkDF & 0x20) {
            func_80063194(obj);
        }
        if (obj->unk16 == 0x24 && obj->unkFE != 0 && !(obj->flags & 0x4000000)) {
            kind = (obj->unkFE == 1) ? obj->unkF7 : obj->unkF6;
            if ((*(s32 *)0x8009FA0C) == 1) {
                {
                    register char *base __asm__("$5") = (char *)0x800A0000;
                    __asm__ volatile ("" : "+r" (base));
                    p = *(struct Unk2D664State **)(base - 0x614);
                }
                if (p != 0 && p->unk18 == kind) {
                    (*(struct Unk2D664Menu **)0x8009FA94)->unk68++;
                    (*(struct Unk2D664Menu **)0x8009FA94)->unk80++;
                    __asm__ volatile ("" : : : "memory");
                    D_8009FA58[0]->unkAC->unk10++;
                    p->unk134->unk18 = 0;
                    found = func_8004D664(obj->unk8C & 0x3FFFFFFF);
                    if (found != 0) {
                        if ((found->unk14 & 0x3FFF) == (*(struct Unk2D664State **)0x8009F9EC)->unk14) {
                            register char *base __asm__("$3") = (char *)0x800A0000;
                            __asm__ volatile ("" : "+r" (base));
                            (*(struct Unk2D664Menu **)(base - 0x56C))->unkE0 = 7;
                        }
                    }
                    return func_8002AD58(message, obj, arg2, arg3);
                }
            }
            menu = (*(struct Unk2D664Menu **)0x8009FA94);
            st = (*(struct Unk2D664State **)0x8009F9EC);
            __asm__ ("lui\t%0,%%hi(D_8009FA58)\n\taddiu\t%0,%0,%%lo(D_8009FA58)" : "=r" (players));
            i = 0;
            do {
                if (players[i] != 0 && players[i]->unkAC != 0 && players[i]->unk18 == kind) {
                    ((s32 *)((u8 *)menu + 0x64))[players[i]->unkAC->unkC1]++;
                    ((s32 *)((u8 *)menu + 0x7C))[players[i]->unkAC->unkC1]++;
                    if (st != 0) {
                        st->unk134->unk38++;
                        menu->unkE0 = 8;
                    } else {
                        players[i ^ 1]->unkAC->unk10++;
                    }
                }
            } while (++i < 2);
        }
        break;
    default:
        break;
    }
    return func_8002AD58(message, obj, arg2, arg3);
}
