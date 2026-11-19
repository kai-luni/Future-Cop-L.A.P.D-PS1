// Categories: Graphics > Data Animation
typedef unsigned char u8;
typedef struct Node {
    struct Node *next;
    u8 *data;
    int count;
    u8 entry[8];
} Node;

void gUpdateModelWaveBytes(Node *node, int time) __asm__("func_800135C0");

/**
 * @brief Updates three model record bytes from a folded time-dependent phase.
 *
 * @param node Head of the wave chunk list, or null.
 * @param time Clock value used to compute the phase.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Masks time times entry byte 1 to 12 bits and folds the upper half around
 * 0xFFF. Entry byte 0 selects a 12-byte record. For each endpoint pair it
 * multiplies the difference by the folded phase, divides by 2048 with
 * truncation toward zero, and stores bytes +5 through +7. The inner loop
 * reuses the same entry address for the node count.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gStepAnimations.
 *
 * @par Role and path
 * The model animation tick passes the wave chunk list and the current clock
 * value.
 *
 * @par Unknown
 * The exact meaning of each of the three output bytes is open.
 */
void gUpdateModelWaveBytes(Node *node, int time)
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
