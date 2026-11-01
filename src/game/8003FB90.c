#include "game/types.h"

struct UnkFB90Owner {
    u8 unk00[0x14];
    u8 unk14;
};

struct UnkFB90Sub {
    u8 unk00[4];
    void *unk04;
    u8 unk08[0x30];
    u8 unk38;
};

struct UnkFB90Node {
    struct UnkFB90Node *next;
    u8 unk04[0x0B];
    u8 unk0F;
    u8 unk10;
    u8 unk11[0x0B];
    struct UnkFB90Sub *unk1C;
    struct UnkFB90Owner *unk20;
};

struct UnkFB90Global {
    struct UnkFB90Global *next;
    u8 unk04;
    u8 unk05[0x0B];
    void *unk10;
};

extern struct UnkFB90Node *D_800A1070[7];
extern struct UnkFB90Global *D_8009F30C;

extern void func_8003CA80(void *object);
extern void func_800181D4(void *object);

void func_8003FB90(void *object_, s32 kind_)
{
    register void *object __asm__("$19");
    register s32 kind __asm__("$21");
    register struct UnkFB90Global *previous __asm__("$20");
    struct UnkFB90Node *node;
    struct UnkFB90Global *entry;
    struct UnkFB90Global *next;
    s32 i;

    object = object_;
    kind = kind_;
    previous = 0;
    for (i = 0; i < 7; i++) {
        node = D_800A1070[i];
        while (node != 0) {
            if (node->unk20->unk14 == kind &&
                ((node->unk1C != 0 && node->unk1C->unk38 == 0)
                    ? node->unk1C->unk04 == object
                    : object == 0)) {
                if (node->unk10 & 4) {
                    func_8003CA80(node);
                } else {
                    node->unk0F |= 1;
                }
            }
            node = node->next;
        }
    }

    entry = D_8009F30C;
    while (entry != 0) {
        next = entry->next;
        if (entry->unk10 == object && entry->unk04 == kind) {
            if (previous != 0) {
                previous->next = next;
            } else {
                D_8009F30C = next;
            }
            entry->next = 0;
            func_800181D4(entry);
        } else {
            previous = entry;
        }
        entry = next;
    }
}
