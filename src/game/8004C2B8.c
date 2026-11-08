#include "game/types.h"

struct Unk4AADParent {
    u8 unk00[0x39];
    u8 flags;
    u16 count;
};

struct Unk4AADNode {
    u8 unk00[0x28];
    u8 flags;
    u8 unk29[0x1F];
    struct Unk4AADNode *next;
    struct Unk4AADParent *parent;
    u8 unk50[8];
};

struct Unk4AADOwner {
    u8 unk00[0x84];
    struct Unk4AADNode *head;
};

struct Unk4C2B8Sub3 {
    u8 unk00[0x0C];
    u32 flags;
};

struct Unk4C2B8Sub2 {
    struct Unk4C2B8Sub3 *unk00;
};

struct Unk4C2B8Data {
    u8 unk00[0x2C];
    s32 data[9];
};

struct Unk4C2B8Link {
    u8 unk00[0x04];
    struct Unk4C2B8Data *data;
    u8 unk08[0x2C];
    struct Unk4C2B8Sub2 *sub;
    u8 flag38;
};

struct Unk4C2B8Owner {
    struct Unk4C2B8Owner *next;
    u8 unk04[0x08];
    s16 unk0C;
    u8 unk0E[0x08];
    s16 unk16;
    u8 unk18[0x04];
    struct Unk4C2B8Link *volatile link;
    u8 unk20[0x5B];
    u8 flags;
    u8 unk7C[0x04];
    void *unk80;
    struct Unk4AADNode *head;
};

extern s32 D_8009AA38[9];
extern s32 D_8009F9DC;

extern void func_800181D4(void *block);
extern void func_8003A8C0(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern s32 func_8003C7B8(s32 arg0, struct Unk4C2B8Owner *arg1, s32 arg2, s32 arg3);
extern s32 func_8003C980(s32 message, void *object, s32 arg2, s32 arg3);
extern struct Unk4AADNode *func_8004AAD4(struct Unk4AADOwner *owner, struct Unk4AADNode *previous, struct Unk4AADNode *node);
extern s32 func_8004ABE4(struct Unk4C2B8Owner *object, s32 arg1);
extern void func_8004B180(void *object);
extern s32 func_8004B5F8(struct Unk4C2B8Owner *object, s32 arg1);
extern void func_8004C0C0(struct Unk4C2B8Owner *object);

void func_8004C2B8(struct Unk4C2B8Owner *input)
{
    register struct Unk4C2B8Owner *node __asm__("$16");
    node = input;
    while (node != 0) {
        u8 flags = node->flags;
        if (flags & 0x08) {
            func_8004B180(node);
        } else if (!(flags & 0x10)) {
            func_8004C0C0(node);
        }
        node = node->next;
    }
}

