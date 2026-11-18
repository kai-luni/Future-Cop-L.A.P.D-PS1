// Categories: Platform > Sound > Sequencing | Game Engine > Frame
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
extern State *gMissionTable __asm__("D_8009F930");
extern void *gMissionState __asm__("D_8009F934");
extern Bucket *gBucketArray __asm__("D_8009F938");
extern int gMissionGridHeight __asm__("D_8009F940");
extern int gMissionGridWidth __asm__("D_8009F93C");
extern int gFrameTime __asm__("D_8009FAA4");
extern void func_800457B8(int, int);
extern void func_800195D0(void *, Node *);
/**
 * @brief Drains the scheduled sound-event buckets whose time has come,
 *        firing due events and freeing their nodes.
 *
 * @param arg0 Current time threshold for the bucket scan.
 * @param arg1 Non-zero stops after the first fired event.
 * @return Nothing.
 *
 * @par Confirmed
 * With D_8009F938 null nothing runs. Buckets are scanned backwards over
 * D_8009F93C * D_8009F940 entries; buckets with time > arg0 are skipped.
 * Per node (until a 0x80 flag or a negative time): nodes due at
 * D_8009FAA4 with active set and index >= 0 raise the state mask bit
 * (D_8009F930->mask | 1 << index) and play func_800457B8(sound |
 * 0x40000000, the per-index byte at state + index * 8 + 10); the node is
 * unlinked (head or middle) and freed via func_800195D0(D_8009F934, node).
 *
 * @par Likely role
 * The sequencer's event pump (called by func_80083A10): the 0x40000000
 * flag marks scheduled playback, the mask tracks which channels fired,
 * and the single-event mode services latency-critical paths.
 *
 * @par Unknown
 * The bucket grid meaning of D_8009F93C/D_8009F940.
 *
 * @par Callers
 * func_80083A10.
 */
void func_80083834(int arg0, unsigned char arg1)
{
    int i, index, offset, mask;
    Node *node, *prev, *head, *next;
    State *state;
    if (gBucketArray == 0) return;
    state = gMissionTable;
    for (i = gMissionGridWidth * gMissionGridHeight - 1; i >= 0; i--) {
        if (arg0 < gBucketArray[i].time) continue;
        node = gBucketArray[i].head;
        while (node != 0 && !(node->flags & 0x80)) {
            if (node->time < 0) break;
            if (node->time <= gFrameTime) {
                if (node->active && (index = node->index) >= 0) {
                    mask = state->mask | (1 << index);
                    __asm__("" : "+r"(index) : "r"(mask));
                    state->mask = mask;
                    offset = index * 8 + 4;
                    __asm__("" : "+r"(offset));
                    func_800457B8(node->sound | 0x40000000, *(signed char *)((char *)state + offset + 6));
                }
                head = gBucketArray[i].head;
                if (node == head) {
                    next = node->next;
                    __asm__("" : "+r"(next));
                    node = next;
                    gBucketArray[i].head = node;
                    func_800195D0(gMissionState, head);
                    continue;
                }
                prev = head;
                while (prev->next != node) prev = prev->next;
                prev->next = node->next;
                func_800195D0(gMissionState, node);
                node = prev;
                if (arg1) return;
            }
            node = node->next;
        }
    }
}
