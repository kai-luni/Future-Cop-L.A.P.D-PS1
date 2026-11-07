typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct Node {
    u16 *data;
    u8 pad04[0x14];
    u32 tag;
} Node;

extern u8 D_8009F26C;
extern Node *D_8009F270;
extern Node *D_8009F274;
extern Node *D_8009F640[2];

extern void func_800181D4(Node *node);
extern void func_800236FC(void);

void func_80023930(Node *node)
{
    s32 i;
    register Node **p __asm__("$3");

    if (D_8009F26C != 0) {
        switch (node->tag) {
        case 0x43766B68:
            D_8009F270 = node;
            break;

        case 0x43766B62:
            D_8009F274 = node;
            break;

        case 0x43736864:
            if (*node->data != 4) {
                func_800181D4(node);
                break;
            }

            i = 0;
            p = D_8009F640;
            for (; i < 2; i++, p++) {
                if (*p == 0) {
                    *p = node;
                    break;
                }
            }
            if (i == 2) {
                func_800181D4(node);
            }
            break;

        default:
            func_800181D4(node);
            break;
        }
        func_800236FC();
    } else {
        func_800181D4(node);
    }
}
