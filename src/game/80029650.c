#include "game/types.h"

typedef struct Node {
    struct Node *next;
    s32 pad[2];
    s32 count;
} Node;

typedef struct {
    u8 pad[0x18];
    Node *head;
} List;

extern List D_800A0B70;
extern void func_800288BC(Node *node);

void func_80029650(Node **arg0)
{
    Node *current;
    Node *node;
    List *list;

    (*arg0)->count--;
    __asm__ volatile("" : : : "memory");
    list = &D_800A0B70;
    __asm__ volatile("" : "+r"(list));
    current = list->head;
    if (current != 0) {
        while (1) {
            if (*(Node *volatile *)&current->next != 0) {
                node = current->next;
                if (node->count == 0) {
                    current->next = node->next;
                    node->next = 0;
                    func_800288BC(node);
                } else {
                    current = node;
                }
            } else {
                node = list->head;
                if (node->count == 0) {
                    list->head = node->next;
                    node->next = 0;
                    func_800288BC(node);
                }
                break;
            }
        }
    }
}
