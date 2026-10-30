#include "game/types.h"

struct Node24EC8 {
    u8 pad00[0xC];
    struct Node24EC8 *next;
    struct Node24EC8 *prev;
    u8 pad14[4];
    u32 tag;
    s32 unk1C;
    u8 pad20[4];
    s32 unk24;
    s32 unk28;
    s32 unk2C;
};

extern struct Node24EC8 *D_8009F668;
extern struct Node24EC8 *D_8009F66C;
extern s32 D_8009F9F4;
extern s32 D_8009FA00;

extern struct Node24EC8 *func_800295E8(s32 arg0);
extern void func_80013550(struct Node24EC8 *node);
extern void func_800181D4(struct Node24EC8 *node);
extern void func_80023930(struct Node24EC8 *node);
extern void func_80029E20(struct Node24EC8 *node);
extern void func_8003C110(struct Node24EC8 *node);
extern void func_80048E9C(struct Node24EC8 *node);
extern void func_80049420(struct Node24EC8 *node);
extern void func_80049734(struct Node24EC8 *node);
extern void func_8004D6D4(struct Node24EC8 *node);
extern void func_8004D924(struct Node24EC8 *node);
extern void func_800511E4(struct Node24EC8 *node);
extern void func_8005A414(struct Node24EC8 *node);
extern void func_80060E74(struct Node24EC8 *node);
extern void func_80063C20(struct Node24EC8 *node);
extern void func_80063C5C(struct Node24EC8 *node);
extern void func_80063DC8(struct Node24EC8 *node);
extern void func_80082358(struct Node24EC8 *node);
extern void func_800824CC(struct Node24EC8 *node);
extern void func_800825AC(struct Node24EC8 *node);
extern void func_8008263C(struct Node24EC8 *node);
extern void func_800899A4(struct Node24EC8 *node);

void func_80024EC8(void)
{
    struct Node24EC8 *node;
    struct Node24EC8 *next;
    s32 flag;
    s32 mode;

    flag = 1;
    goto loop_test;
loop_body:
    node->next = 0;
    node->prev = 0;
    if (D_8009F66C != 0) {
        D_8009F66C->next = node;
        node->prev = D_8009F66C;
    } else {
        D_8009F668 = node;
    }
    D_8009F66C = node;
    flag = 0;
loop_test:
    node = func_800295E8(flag);
    if (node != 0) {
        goto loop_body;
    }

    if (D_8009F668 != 0) {
      node = D_8009F668;
      do {
        next = node->next;
        if (D_8009FA00 != 4) {
            if (node->prev != 0) {
                node->prev->next = next;
            } else {
                D_8009F668 = next;
                if (next != 0) {
                    next->prev = 0;
                }
            }

            if (node->next != 0) {
                node->next->prev = node->prev;
            } else {
                D_8009F66C = node->prev;
                if (D_8009F66C != 0) {
                    D_8009F66C->next = 0;
                }
            }

            node->unk24 += D_8009F9F4;
            node->unk28 += D_8009F9F4;
            node->unk2C += D_8009F9F4;

            switch (node->tag) {
            case 0x43747273:
                func_800899A4(node);
                break;
            case 0x4374696E:
                func_800825AC(node);
                break;
            case 0x43746474:
                func_800824CC(node);
                break;
            case 0x436D6463:
                func_8008263C(node);
                break;
            case 0x4361746C:
            case 0x4374696C:
                func_80063C20(node);
                break;
            case 0x43626D70:
                func_80063C5C(node);
                break;
            case 0x43707463:
                func_80063DC8(node);
                break;
            case 0x43666E74:
                func_80048E9C(node);
                break;
            case 0x43637472:
                switch (node->unk1C) {
                case 0:
                    mode = 2;
                    break;
                case 1:
                    mode = 4;
                    break;
                default:
                    goto default_action;
                }
                D_8009FA00 = mode;
                goto default_action;
            case 0x436F626A:
                func_80013550(node);
                break;
            case 0x43616374:
                func_8004D6D4(node);
                break;
            case 0x52504E53:
                func_8004D924(node);
                break;
            case 0x43736163:
                func_80060E74(node);
                break;
            case 0x4366756E:
                func_80049420(node);
                break;
            case 0x43736864:
            case 0x43766B68:
            case 0x43766B62:
                func_80023930(node);
                break;
            case 0x436D6963:
                func_80082358(node);
                break;
            case 0x43746F73:
                func_80029E20(node);
                break;
            case 0x436E6574:
                func_800511E4(node);
                break;
            case 0x43646373:
                func_80049734(node);
                break;
            case 0x43707972:
                func_8005A414(node);
                break;
            case 0x43736678:
                func_8003C110(node);
                break;
            case 0x436E6F6E:
                break;
            default:
            default_action:
                func_800181D4(node);
                break;
            }
        }
        node = next;
      } while (node != 0);
    }
}
