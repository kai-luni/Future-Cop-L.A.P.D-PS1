#include "game/types.h"

struct Unk3BObject;

typedef struct UnkFAD4Node {
    struct UnkFAD4Node *next;
    struct Unk3BObject *object;
} UnkFAD4Node;

typedef struct UnkFAD4Local {
    u32 color[2];
    s16 rect[4];
} UnkFAD4Local;

extern UnkFAD4Node *D_8009F708;
extern s32 D_8009F70C;
extern s32 D_8009F9B8;
extern s32 D_8009FAA0;

extern void func_8003C590(s32 arg0);
extern void func_8003BC0C(struct Unk3BObject *object);
extern void func_80019584(s32 arg0);
extern void func_80092F38(s16 *rect, u32 *color);
extern void func_80092C8C(s32 arg0);

void func_8003FAD4(void)
{
    UnkFAD4Node *node = D_8009F708;
    UnkFAD4Local local;

    func_8003C590(0);
    func_8003C590(1);

    while (node != 0) {
        func_8003BC0C(node->object);
        node = node->next;
    }

    func_80019584(D_8009F70C);

    local.color[0] = 0xC210C210;
    local.color[1] = 0xC210C210;
    D_8009F9B8 = 0x13E;
    D_8009FAA0 = 0x15E;
    local.rect[0] = 0x380;
    local.rect[1] = 0x100;
    D_8009F70C = 0;
    D_8009F708 = 0;
    local.rect[2] = 2;
    local.rect[3] = 2;

    func_80092F38(local.rect, local.color);
    func_80092C8C(0);
}
