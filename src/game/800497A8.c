// Categories: Graphics > Lighting | Game Engine > Object Pool
typedef struct Node {
    int unk0;
    int unk4;
    unsigned char unk8;
    unsigned char pad[0x37];
    struct Node *next;
} Node;

extern Node **gLightGrid __asm__("D_8009F780");
extern void *gLightNodePool __asm__("D_8009F784");
extern int gLightGridWidth __asm__("D_8009F78C");
extern int gLightGridHeight __asm__("D_8009F790");
extern int gDiscShadeByte __asm__("D_8009FAA4");
extern void func_800195D0(void *, Node *);

/**
 * @brief Reaps expired dynamic light nodes across the spatial bucket grid D_8009F780.
 *
 * Iterates buckets i = D_8009F78C * D_8009F790 - 1 down to 0, walking each bucket's
 * sorted singly-linked list while node->unk4 >= 0 and !(node->unk8 & 0x40), unlinking
 * every node with expiry tick node->unk4 <= D_8009FAA4 and returning it to pool D_8009F784.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F780 - grid array of light-list heads (D_8009F78C * D_8009F790 buckets);
 * D_8009F784 - light node pool handle; D_8009FAA4 - current game tick threshold.
 * @par Control flow
 * Null guard on D_8009F780, reverse bucket loop, inner while stopping at unk4 < 0 or bit 0x40, head-or-prev unlink.
 * @par Calls
 * func_800195D0 - returns each unlinked light node to pool D_8009F784 (direct jal at 0x80049840 and 0x80049880 in asm/us/main.s).
 * @par Side effects
 * Unlinks expired nodes from D_8009F780 buckets and frees them back into D_8009F784.
 * @par Callers
 * Confirmed: func_80049C38 (light pool exhaustion retry); also direct call at 0x8004A97C in asm/us/main.s (periodic sweep).
 */
void func_800497A8(void)
{
    int i;
    register Node *node __asm__("$17");
    register Node *prev __asm__("$16");
    register Node *next __asm__("$2");
    register Node *head __asm__("$5");

    if (gLightGrid != 0) {
        for (i = gLightGridWidth * gLightGridHeight - 1; i >= 0; i--) {
            node = gLightGrid[i];
            while (node != 0 && node->unk4 >= 0 && !(node->unk8 & 0x40)) {
                if (node->unk4 <= gDiscShadeByte) {
                    head = gLightGrid[i];
                    prev = head;
                    if (node == head) {
                        next = node->next;
                        __asm__ volatile("" : "+r"(next));
                        node = next;
                        gLightGrid[i] = node;
                        func_800195D0(gLightNodePool, head);
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
                    func_800195D0(gLightNodePool, head);
                }
                node = node->next;
            }
        }
    }
}
