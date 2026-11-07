typedef struct Node Node;
struct Node {
    char pad0[0x4b];
    signed char index;
    unsigned char flags;
    char pad4d[3];
    int time;
    unsigned short sound;
    char pad56;
    unsigned char active;
    Node *next;
};
typedef struct Bucket { Node *head; int time; } Bucket;
typedef struct Entry { char pad[6]; signed char index; char pad7; } Entry;
typedef struct State { int mask; Entry entries[1]; } State;
extern State *D_8009F930;
extern void *D_8009F934;
extern Bucket *D_8009F938;
extern int D_8009F93C, D_8009F940;
extern int D_8009FAA4;
extern void func_800457B8(int, int);
extern void func_800195D0(void *, Node *);
void func_80083834(int arg0, unsigned char arg1)
{
    int i, index, offset, mask;
    Node *node, *prev, *head, *next;
    State *state;
    if (D_8009F938 == 0) return;
    state = D_8009F930;
    for (i = D_8009F93C * D_8009F940 - 1; i >= 0; i--) {
        if (arg0 < D_8009F938[i].time) continue;
        node = D_8009F938[i].head;
        while (node != 0 && !(node->flags & 0x80)) {
            if (node->time < 0) break;
            if (node->time <= D_8009FAA4) {
                if (node->active && (index = node->index) >= 0) {
                    mask = state->mask | (1 << index);
                    __asm__("" : "+r"(index) : "r"(mask));
                    state->mask = mask;
                    offset = index * 8 + 4;
                    __asm__("" : "+r"(offset));
                    func_800457B8(node->sound | 0x40000000, *(signed char *)((char *)state + offset + 6));
                }
                head = D_8009F938[i].head;
                if (node == head) {
                    next = node->next;
                    __asm__("" : "+r"(next));
                    node = next;
                    D_8009F938[i].head = node;
                    func_800195D0(D_8009F934, head);
                    continue;
                }
                prev = head;
                while (prev->next != node) prev = prev->next;
                prev->next = node->next;
                func_800195D0(D_8009F934, node);
                node = prev;
                if (arg1) return;
            }
            node = node->next;
        }
    }
}
