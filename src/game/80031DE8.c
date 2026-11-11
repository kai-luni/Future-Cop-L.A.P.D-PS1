#include "game/types.h"

typedef struct Target80031DE8 Target80031DE8;
typedef struct Node80031DE8 Node80031DE8;

struct Target80031DE8 {
    u8 unk00[8];
    s32 id;
    u8 unk0C[8];
    u16 nodeId;
    u8 type;
    u8 unk17[0x39];
    s32 x;
    s32 unk54;
    s32 y;
};

struct Node80031DE8 {
    s16 x;
    s16 unk02;
    s16 y;
    s16 unk06;
    u8 unk08[0x0A];
    s16 id;
    u8 flags;
    u8 unk15;
    u16 timer;
    u8 unk18[4];
    Target80031DE8 *target;
    s32 targetId;
    Node80031DE8 *next;
};

typedef struct {
    s32 count;
    s32 unk04;
    Target80031DE8 *targets[1];
} Manager80031DE8;

extern Node80031DE8 *D_8009F6F8;
extern Manager80031DE8 D_800A8F90;
extern Target80031DE8 *D_8009FA58[2];

extern s32 func_80019668(void);
extern s32 func_80031C74(Node80031DE8 *node, Target80031DE8 *target);
extern void func_800181D4(void *block);

void func_80031DE8(s32 delta)
{
    Node80031DE8 *node;
    Node80031DE8 *prev;
    Target80031DE8 *target;
    Target80031DE8 *candidate;
    s32 bestDist;
    s32 i;
    s32 dx;
    s32 dy;
    register s32 min __asm__("$5");
    s32 tmp;
    s32 r;
    s32 t;

    node = D_8009F6F8;
    prev = 0;
    while (node != 0) {
    if (node->unk15 != 0) {
        goto remove;
    }
    if (node->flags & 0x10) {
        goto next;
    }
    if (node->id != -1) {
        if (node->target != 0) {
            if (node->targetId != node->target->id) {
                node->target = 0;
                node->timer = 0x258;
                goto next;
            }
        }
        if (node->timer < delta) {
            target = 0;
            for (i = 0; i < D_800A8F90.count; i++) {
                candidate = D_800A8F90.targets[i];
                if (candidate->id > 0 &&
                    candidate->nodeId == node->id &&
                    candidate->type != 0x15 &&
                    candidate->type != 0x12) {
                    dx = candidate->x - (node->x << 8);
                    dx = ((dx >> 31) + dx) ^ (dx >> 31);

                    dy = candidate->y - (node->y << 8);
                    dy = ((dy >> 31) + dy) ^ (dy >> 31);

                    min = dx;
                    if (dy < dx) {
                        min = dy;
                    }
                    dx += dy - min / 2;
                    if (target == 0 || dx < bestDist) {
                        target = candidate;
                        bestDist = dx;
                        __asm__("" : : "r"(min));
                    }
                }
            }
            if (node->target != target) {
                node->target = target;
                if (target != 0) {
                    node->targetId = target->id;
                }
            }
            r = func_80019668();
            t = node->timer + 0x258;
            node->timer = t + (r & 0x3F);
            if (target == 0) {
                goto next;
            }
            node->timer += 0x258;
        }
        node->timer -= delta;
        target = node->target;
        goto check;
    }

    i = 0;
load_target:
    target = D_8009FA58[i];

check:
    if (target == 0) {
        goto after_call;
    }
    if ((func_80031C74(node, target) & 0xFF) == 0) {
        goto after_call;
    }

remove:
    if (prev != 0) {
        prev->next = node->next;
    } else {
        D_8009F6F8 = node->next;
    }
    func_800181D4(node);
    node = prev;
    goto next;

after_call:
    if (node->id != -1) {
        goto next;
    }
    i++;
    if (i >= 2) {
        goto next;
    }
    do {
        if (D_8009FA58[i] != 0) {
            goto load_target;
        }
        i++;
    } while (i < 2);

next:
    if (node == 0) {
        break;
    }
    prev = node;
    node = node->next;
    }
}
