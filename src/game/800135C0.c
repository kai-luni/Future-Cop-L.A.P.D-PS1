typedef unsigned char u8;
typedef struct Node {
    struct Node *next;
    u8 *data;
    int count;
    u8 entry[8];
} Node;

void func_800135C0(Node *node, int time)
{
    register int count __asm__("$10");
    register int phase __asm__("$7");
    register u8 *p __asm__("$8");
    register u8 *dest __asm__("$9");
    register u8 *base __asm__("$11");
    register int index __asm__("$3");
    register int offset __asm__("$2");
    register int value __asm__("$6");
    register int top __asm__("$3");
    register int limit __asm__("$12");
    if (!node) return;
    limit = 0xfff;
    while (node) {
        count = node->count;
        base = node->data;
        p = node->entry;
        while (count > 0) {
            offset = p[1];
            __asm__("" : "+r"(offset));
            phase = time * offset;
            index = p[0];
            __asm__("" : "+r"(index));
            offset = index * 12;
            __asm__("" : "+r"(offset));
            dest = base + offset;
            __asm__("" : "+r"(phase));
            phase &= 0xfff;
            if (phase >= 0x800)
                phase = limit - phase;
            value = p[2];
            top = p[5];
            offset = value - 1;
            __asm__("" : "+r"(offset));
            offset = value + ((top - offset) * phase) / 2048;
            __asm__("" : "+r"(offset));
            dest[5] = offset;
            value = p[3];
            top = p[6];
            offset = value - 1;
            __asm__("" : "+r"(offset));
            offset = value + ((top - offset) * phase) / 2048;
            __asm__("" : "+r"(offset));
            dest[6] = offset;
            value = p[4];
            top = p[7];
            offset = value - 1;
            __asm__("" : "+r"(offset));
            offset = value + ((top - offset) * phase) / 2048;
            __asm__("" : "+r"(offset));
            dest[7] = offset;
            count--;
        }
        node = node->next;
    }
}
