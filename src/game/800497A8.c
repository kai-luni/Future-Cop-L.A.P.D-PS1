typedef struct Node {
    int unk0;
    int unk4;
    unsigned char unk8;
    unsigned char pad[0x37];
    struct Node *next;
} Node;

extern Node **D_8009F780;
extern void *D_8009F784;
extern int D_8009F78C;
extern int D_8009F790;
extern int D_8009FAA4;
extern void func_800195D0(void *, Node *);

void func_800497A8(void)
{
    int i;
    register Node *node __asm__("$17");
    register Node *prev __asm__("$16");
    register Node *next __asm__("$2");
    register Node *head __asm__("$5");

    if (D_8009F780 != 0) {
        for (i = D_8009F78C * D_8009F790 - 1; i >= 0; i--) {
            node = D_8009F780[i];
            while (node != 0 && node->unk4 >= 0 && !(node->unk8 & 0x40)) {
                if (node->unk4 <= D_8009FAA4) {
                    head = D_8009F780[i];
                    prev = head;
                    if (node == head) {
                        next = node->next;
                        __asm__ volatile("" : "+r"(next));
                        node = next;
                        D_8009F780[i] = node;
                        func_800195D0(D_8009F784, head);
                        continue;
                    }
                    if (prev->next != node) {
                        __asm__ volatile("" : : : "memory");
                        do {
                            prev = prev->next;
                        } while (prev->next != node);
                    }
                    __asm__ volatile("" : "+r"(node));
                    head = node;
                    node = prev;
                    node->next = head->next;
                    func_800195D0(D_8009F784, head);
                }
                node = node->next;
            }
        }
    }
}
