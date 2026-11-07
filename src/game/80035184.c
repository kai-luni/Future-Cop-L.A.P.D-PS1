typedef struct Node {
    struct Node *next;
    char pad04[4];
    int active;
    char pad0c[10];
    unsigned char type;
    char pad17[0x39];
    int x;
    int y;
    int z;
    char pad5c[0x6c];
    int flags;
} Node;
extern Node *D_8009FA40;

Node *func_80035184(Node *obj, int *distance)
{
    register Node *node __asm__("$9");
    register Node *best __asm__("$10") = 0;
    register int bestDist __asm__("$11");
    int x;
    register int z __asm__("$6");
    register int min __asm__("$8");
    register int sign __asm__("$3");
    register int dist __asm__("$7");
    register int half __asm__("$2");
    for (node = D_8009FA40; node != 0; node = node->next) {
        if (node->active && node->type == 16 && (node->flags & 0x20000)) {
            x = obj->x - node->x;
            sign = x >> 31;
            x = (sign + x) ^ sign;
            sign = obj->z;
            half = node->z;
            z = sign - half;
            sign = z >> 31;
            half = sign + z;
            z = half ^ sign;
            min = x;
            if (z < x) min = z;
            half = min / 2;
            dist = x + (z - half);
            if (!best || dist < bestDist) {
                best = node;
                bestDist = dist;
            }
        }
    }
    if (best) *distance = bestDist;
    return best;
}
