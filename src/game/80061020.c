#include "game/types.h"

struct Def {
    s32 id;
    u8 pad04[0x18];
    u8 team;
    u8 type;
};

struct Info {
    u8 pad00[0x16];
    u16 unk16;
    u8 pad18[0x10];
    struct Def *def;
};

struct Entry {
    s32 unk00;
    struct Info *info;
};

struct Table {
    s32 count;
    struct Entry *entries[1];
};

struct Sub134 {
    u8 pad00[0x18];
    u16 unk18;
    u8 pad1A[0xE];
    u16 unk28;
    u16 pad2A;
    u16 unk2C;
    u16 pad2E;
    u16 unk30;
    u16 pad32;
    u16 unk34;
};

struct Unit {
    u8 pad00[0x14];
    u16 unk14;
    u8 pad16[2];
    u8 team;
    u8 pad19[0x11B];
    struct Sub134 *sub134;
};

struct SubAC {
    u16 unk00;
    u16 pad02;
    u16 unk04;
    u16 pad06;
    u16 unk08;
    u16 pad0A;
    u16 unk0C;
};

struct Actor {
    u8 pad00[0x18];
    u8 team;
    u8 pad19[0x93];
    struct SubAC *unkAC;
};

struct GlobalFA94 {
    u8 pad00[0x5C];
    s32 unk5C;
};

extern struct Table *D_8009F800;
extern struct Unit *D_8009F9EC;
extern struct GlobalFA94 *D_8009FA94;
extern struct Actor *D_8009FA58[];

void func_80061020(struct Unit *arg0)
{
    struct Table *table;
    register struct Entry **cursor __asm__("$8");
    struct Entry *entry;
    struct Info *info;
    struct Def *def;
    register struct Unit *unit __asm__("$5");
    struct Actor *actor;
    struct SubAC *sub;
    register s32 remaining __asm__("$7");
    register s32 targetId __asm__("$10");
    register s32 one __asm__("$9");

    table = D_8009F800;
    targetId = arg0->unk14;
    cursor = table->entries;
    remaining = table->count;
    if (remaining > 0) {
        one = 1;
        __asm__ volatile ("" : : "r"(one));
        unit = D_8009F9EC;
        do {
            entry = *cursor++;
            info = entry->info;
            if (info->def->id != targetId) {
                continue;
            }

            info->unk16--;

            if (D_8009FA94->unk5C != 1) {
                continue;
            }

            def = info->def;

            if (unit != 0) {
                if (unit->team == def->team) {
                    sub = 0;
                } else {
                    sub = D_8009FA58[0]->unkAC;
                }
            } else if (def->team == D_8009FA58[0]->team) {
                sub = D_8009FA58[0]->unkAC;
            } else {
                sub = D_8009FA58[1]->unkAC;
            }
            switch (def->type) {
            case 1:
                if (sub != 0) {
                    sub->unk00++;
                } else {
                    if (unit != 0) {
                        unit->sub134->unk28++;
                    }
                    break;
                }
                goto set_zero;

            case 2:
                if (sub != 0) {
                    sub->unk04++;
                } else {
                    if (unit != 0) {
                        unit->sub134->unk2C++;
                    }
                    break;
                }
                goto set_zero;

            case 3:
                if (sub != 0) {
                    sub->unk08++;
                } else {
                    if (unit != 0) {
                        unit->sub134->unk30++;
                    }
                    break;
                }
                goto set_zero;

            case 4:
                if (sub == 0) {
                    goto case_4_else;
                }
                sub->unk0C++;

            set_zero:
                if (unit != 0) {
                    unit->sub134->unk18 = 0;
                }
                break;

            case_4_else:
                if (unit != 0) {
                    unit->sub134->unk34++;
                }
                break;
            }
        } while (--remaining > 0);
    }
}
