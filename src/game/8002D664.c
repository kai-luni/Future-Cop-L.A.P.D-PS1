// Categories: Gameplay > Entity Script | Game Engine > Message Dispatch | Gameplay > Scoring
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

extern s32 gPlayerCount __asm__("D_8009FA0C");
extern struct Unk2D664State *gSinglePlayerEntity __asm__("D_8009F9EC");
extern struct Unk2D664Menu *gGameSession __asm__("D_8009FA94");
extern struct Unk2D664Player *gPlayerEntities[2] __asm__("D_8009FA58");

extern s32 gEntityBaseHandler(s32 message, struct Unk2D664Obj *object, void *data,
                         s32 arg3) __asm__("func_8002AD58");
extern void gInitObjectFromDef(struct Unk2D664Obj *obj, void *data, s32 arg2) __asm__("func_8002C584");
extern s32 gUpdatePropTick(struct Unk2D664Obj *obj, void *data) __asm__("func_8002CC30");
extern void gTryCaptureObjective(struct Unk2D664Obj *obj, void *data) __asm__("func_8002D1A0");
extern void gRemoveGroundKeyEntry(s32 key) __asm__("func_80063194");
extern struct Unk2D664Entity *gFindUnitById(s32 id) __asm__("func_8004D664");
s32 gHandleLinkedPropEntity(s32 message, struct Unk2D664Obj *obj, void *arg2, s32 arg3) __asm__("func_8002D664");


/**
 * @brief Message dispatcher for multi-part/objective entity classes (6, 8, 30, 33, 36).
 *
 * Reports size 0x118 on message 1, handles setup/tick/capture on messages 2/6/10, propagates hit flags and health onto child parts unk104/unk100, and records capture/destruction scoring on message 5 before delegating to func_8002AD58.
 * @param message Message id (1 size, 2 spawn, 4 retire, 5 free/score, 6 tick, 9/11/14 child sync, 10 capture/identity).
 * @param obj Entity block (+0x16 kind, +0x1C value, +0x24 flags, +0x8C attacker, +0xDF/+0xF6..+0xFE capture fields, +0x100/+0x104 children).
 * @param arg2 Message payload (child pointer comparison in 10, capture partner in 10 when kind == 0x24).
 * @param arg3 Extra parameter forwarded to func_8002C584 and func_8002AD58.
 * @return 0x118 for message 1, 0 for 2/9/11/14, child-match flag for 10, or result of func_8002CC30 / func_8002AD58.
 *
 * @par Globals
 * D_8009FA0C (player mode), D_8009F9EC (single-player entity), D_8009FA94 (match/menu counters +0x64/+0x7C/+0xE0), D_8009FA58 (two-slot player array).
 *
 * @par Control flow
 * Switch on message: 2 calls func_8002C584; 6 tail-calls func_8002CC30; 10 runs func_8002D1A0 when unk16 == 0x24 and returns !(arg2 == unk104 || arg2 == unk100); 9/11/14 run func_8002AD58 and sync bit 0x2000000 and unk1C to unk104/unk100; 4 sets bit 8 on children; 5 calls func_80063194 if unkDF & 0x20 and updates scoring/state for active kind-0x24 entities.
 *
 * @par Calls
 * func_8002C584 (setup), func_8002CC30 (tick), func_8002D1A0 (capture check), func_80063194 (remove from ground table), func_8004D664 (resolve attacker), func_8002AD58 (base handler).
 *
 * @par Side effects
 * Updates child flags/health, unregisters from ground table, and increments match/player stats in D_8009FA94, D_8009F9EC, and D_8009FA58.
 *
 * @par Callers
 * Confirmed: func_8002DEA8 (0x8002DF60, messages 11/14 forwarding to child unkB4). Likely: indirect dispatch via handler returned by func_8002AE54 (0x8002AEBC) for types 6, 8, 30, 33, 36.
 *
 * @par Map-object path
 * Steps 7 and 9 of the map-object path (docs/precinct-assault-map-objects.md): the
 * message dispatcher that owns the map-placed base. Message 10 forwards to
 * func_8002D1A0 only for kind 0x24 (the objective class), and message 5 is the
 * scoring half - it books the capture in the match block, credits the player's
 * tracker and raises banner 7, all keyed on the latch +0xFE that
 * func_8002D1A0 set. This is the only place where a capture turns into the
 * "Outposts Claimed"/"Current Points" counters. Messages 9/11/14 keep the child
 * parts (+0x100/+0x104) in step with the parent's health and team.
 */
s32 gHandleLinkedPropEntity(s32 message, struct Unk2D664Obj *obj, void *arg2, s32 arg3)
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
        gInitObjectFromDef(obj, arg2, arg3);
        return 0;
    case 6:
        return gUpdatePropTick(obj, arg2);
    case 10:
        if (obj->unk16 == 0x24) {
            gTryCaptureObjective(obj, arg2);
        }
        ret = 0;
        if (obj->unk104 != arg2) {
            ret = obj->unk100 != arg2;
        }
        return ret;
    case 9:
    case 11:
    case 14:
        gEntityBaseHandler(message, obj, arg2, arg3);
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
            gRemoveGroundKeyEntry(obj);
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
                    gPlayerEntities[0]->unkAC->unk10++;
                    p->unk134->unk18 = 0;
                    found = gFindUnitById(obj->unk8C & 0x3FFFFFFF);
                    if (found != 0) {
                        if ((found->unk14 & 0x3FFF) == (*(struct Unk2D664State **)0x8009F9EC)->unk14) {
                            register char *base __asm__("$3") = (char *)0x800A0000;
                            __asm__ volatile ("" : "+r" (base));
                            (*(struct Unk2D664Menu **)(base - 0x56C))->unkE0 = 7;
                        }
                    }
                    return gEntityBaseHandler(message, obj, arg2, arg3);
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
    return gEntityBaseHandler(message, obj, arg2, arg3);
}
