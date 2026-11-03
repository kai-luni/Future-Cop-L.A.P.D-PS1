#include "game/types.h"

typedef struct Entry Entry;
typedef struct Node Node;

struct Entry {
    void *field0;
    void *field4;
    u16 field8;
    u16 padA;
    u32 padC;
};

struct Node {
    Node *next;
    void *field4;
    void *field8;
    s16 count;
    u16 flags;
    Entry *entries;
    s16 field14;
    u16 field16;
    u16 field18;
    u8 r;
    u8 g;
    u8 b;
    u8 pad1D;
    s16 field1E;
    s16 field20;
    s16 field22;
    u16 field24;
    u8 field26;
    u8 pad27;
    u16 field28;
    u8 pad2A[2];
};

extern Node *D_8009F38C;
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800181D4(void *block);
extern void func_80070710(void *block);
extern void func_80070E48(Node *node, Entry *entry, s32 mode);

void *func_8007377C(void *a0, void *a1, s32 a2, s32 count,
                    u8 r, u8 g, u8 b, s16 divisor,
                    u16 arg9, s16 arg10, s32 arg11, u8 arg12,
                    u16 arg13, s32 flagsArg)
{
    Node *node;
    s32 i;
    s32 j;
    s32 size;
    s32 q0, q1, q2;
    register u32 flags __asm__("$3");
    register u32 tst __asm__("$2");
    u32 rgb;
    register s32 fl __asm__("$17");
    register s32 fa __asm__("$18");
    
    register s32 c11 __asm__("$19");

    c11 = arg11;
    fa = flagsArg;
    fl = fa;
    node = func_80017EBC(0x2C, 1);
    ((u8 *)&rgb)[0] = r;
    ((u8 *)&rgb)[1] = g;
    ((u8 *)&rgb)[2] = b;

    if (node == 0) {
        goto cleanup;
    }

    node->next = 0;
    node->field4 = a0;
    flags = fl & 0xFFFF;
    node->field8 = a1;
    node->count = count;
    tst = fa & 1;

    if (tst) {
        flags |= 0x2000;
    }
    node->flags = flags;

    node->field28 = c11;
    node->field14 = arg10;
    node->field16 = arg9;
    node->field26 = arg12;
    node->field24 = arg13;
    node->r = r;
    node->g = g;
    node->b = b;

    if (divisor < 0x10) {
        node->field1E = 0;
        node->field20 = 0;
        node->field22 = 0;
    } else {
        q0 = (((u8 *)&rgb)[0] << 12) / divisor;
        q1 = (((u8 *)&rgb)[1] << 12) / divisor;
        q2 = (((u8 *)&rgb)[2] << 12) / divisor;
        node->field1E = q0;
        node->field20 = q1;
        node->field22 = q2;
    }

    node->entries = func_80017EBC(count << 4, 1);
    if (node->entries == 0) {
        goto free_node;
    }

    if (node->field4 != 0 && *(s32 *)node->field4 == 0) {
        node->flags &= ~0x8;
    }

    for (i = 0; i < count; i++) {
        node->entries[i].field4 = 0;
        node->entries[i].field0 = 0;
    }

    for (i = 0; i < count; i++) {
        if (node->field14 != 0) {
            node->entries[i].field4 = func_80017EBC((a2 + 1) * 3 * 2, 1);
            if (node->entries[i].field4 == 0) {
                goto fail_entries;
            }
        }

        node->entries[i].field0 = func_80017EBC((a2 + 1) * 3 * 4, 1);
        if (node->entries[i].field0 == 0) {
            goto fail_entries;
        }

        node->entries[i].field8 = a2;
    }

    node->field18 = node->field28;
    for (i = 0; i < node->count; i++) {
        func_80070E48(node, &node->entries[i], 1);
    }

    node->next = D_8009F38C;
    D_8009F38C = node;
    return node;

fail_entries:
    j = 0;
    if (i >= 0) {
        for (; j <= i; j++) {
            if (node->entries[j].field4 != 0) {
                func_800181D4(node->entries[j].field4);
            }
            if (node->entries[j].field0 != 0) {
                func_800181D4(node->entries[j].field0);
            }
        }
    }
    func_800181D4(node->entries);

free_node:
    func_800181D4(node);

cleanup:
    if (a0 != 0) {
        func_80070710(a0);
    }
    if (a1 != 0) {
        func_80070710(a1);
    }
    return 0;
}
