#include "game/types.h"

struct Node3F570 {
    struct Node3F570 *next;
    u8 pad[0x20];
    void (*handler)(s32, struct Node3F570 *, s32, s32);
};

extern struct Node3F570 *D_800A1070[];
extern void *gp_0xF8;
extern struct Node3F570 *gp_0x100;

extern void func_8005E0AC(void *arg);
extern void func_8003D240(void);
extern s32 func_8003F2AC(struct Node3F570 *node, void *arg);
extern void func_8003F530(void *arg);
extern void func_8004C2B8(void *arg);
extern void func_8003F428(void *node, void *arg);
extern void func_8003FF7C(void);
extern void func_8003B44C(struct Node3F570 *node);

void func_8003F570(void *arg)
{
    struct Node3F570 **head;
    struct Node3F570 *node;
    struct Node3F570 *next;
    struct Node3F570 *prev;

    func_8005E0AC(arg);
    func_8003D240();
    for (head = D_800A1070; head < D_800A1070 + 7; head++) {
        prev = 0;
        node = *head;
        while (node != 0) {
            next = node->next;
            if ((func_8003F2AC(node, arg) & 0xFF) != 0) {
                if (prev != 0) {
                    prev->next = node->next;
                } else {
                    *head = node->next;
                }
                node->next = 0;
                node->handler(10, node, 0, 0);
            } else {
                prev = node;
            }
            node = next;
        }
    }
    func_8003F530(arg);
    func_8004C2B8(D_800A1070[4]);
    func_8003F428(D_800A1070[5], arg);
    if (gp_0xF8 != 0) {
        func_8003FF7C();
    }
    while (gp_0x100 != 0) {
        node = gp_0x100;
        next = node->next;
        func_8003B44C(node);
        gp_0x100 = next;
    }
}
