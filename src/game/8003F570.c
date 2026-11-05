#include "game/types.h"

struct Unk3F570Node {
    struct Unk3F570Node *next;
    u8 unk04[0x14];
    void *unk18;
    u8 unk1C[0x08];
    void (*unk24)(s32, struct Unk3F570Node *, s32, s32);
};

extern struct Unk3F570Node *D_800A1070[7];
extern struct Unk3F570Node *D_8009F30C;
extern struct Unk3F570Node *D_8009F314;

extern s32 func_8003F2AC(struct Unk3F570Node *object, s32 arg1);
extern void func_8003B44C(struct Unk3F570Node *object);
extern void func_8003D240(void);
extern void func_8003F530(s32 arg);
extern void func_8003FF7C(void);
extern void func_8004C2B8(struct Unk3F570Node *object);
extern void func_8003F428(struct Unk3F570Node *node, s32 arg);
extern void func_8005E0AC(s32 object);

void func_8003F570(s32 arg0)
{
    struct Unk3F570Node *node;
    struct Unk3F570Node *next;
    struct Unk3F570Node *prev;
    struct Unk3F570Node *tail;
    s32 i;
    s32 zero;

    func_8005E0AC(arg0);
    func_8003D240();

    i = 0;
    do {
        node = D_800A1070[i];
        prev = 0;
        if (node != 0) {
            do {
                next = node->next;
                if (func_8003F2AC(node, arg0) & 0xFF) {
                    if (prev != 0) {
                        prev->next = node->next;
                    } else {
                        D_800A1070[i] = node->next;
                    }
                    zero = 0;
                    node->next = 0;
                    node->unk24(10, node, zero, zero);
                } else {
                    prev = node;
                }
                node = next;
            } while (node != 0);
        }
        i++;
    } while (i < 7);

    func_8003F530(arg0);
    func_8004C2B8(D_800A1070[4]);
    func_8003F428(D_800A1070[5], arg0);
    if (D_8009F30C != 0) {
        func_8003FF7C();
    }
    while (D_8009F314 != 0) {
        tail = D_8009F314->next;
        func_8003B44C(D_8009F314);
        D_8009F314 = tail;
    }
}
