#include "game/types.h"

struct Unk47554Node {
    u8 unk00[0xD];
    s8 voice;
    u8 unk0E[0xE];
    struct Unk47554Node *next;
};

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};

extern u8 D_8009F739;
extern s32 D_8009F32C;
extern struct Unk47554Node *D_8009F744;
extern struct Unk47554Node *D_8009F74C;
extern void *D_8009FA58[2];
extern struct Unk4749CHandle D_800A10C0[];

extern void func_80023E84(void);
extern void func_80024630(s32 arg);
extern void func_80047088(s32 arg0, void *arg1);
extern void func_80046698(void *arg);
extern void func_80021B90(void);
extern void func_80021BD0(s32 voice);
extern s32 func_800474F0(struct Unk4749CHandle *handle);

void func_80047554(struct Unk47554Node *node)
{
    s32 i;
    struct Unk47554Node *entry;
    struct Unk47554Node *next;

    D_8009F739 = 0;
    if (node == 0) {
        func_80023E84();
    }
    func_80024630(0);
    {
        register void **p __asm__("$16");
        i = 0;
        p = D_8009FA58;
        do {
            if (*p != 0) {
                func_80047088(0x27, *p);
            }
            p++;
            i++;
        } while (i < 2);
    }
    while (D_8009F32C != 0) {
        D_8009F32C--;
        func_800474F0(&D_800A10C0[D_8009F32C]);
    }
    while (D_8009F744 != 0) {
        func_80046698(D_8009F744);
    }
    entry = D_8009F74C;
    while (entry != 0) {
        next = entry->next;
        if (entry != node) {
            func_80046698(entry);
        }
        entry = next;
    }
    if (node != 0) {
        func_80021BD0(node->voice);
    } else {
        func_80021B90();
    }
}
