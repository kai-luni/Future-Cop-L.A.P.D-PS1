// Categories: Resources > Node Registration
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct Node {
    u16 *data;
    u8 pad04[0x14];
    u32 tag;
} Node;

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern Node *gPendingCvkhNode __asm__("D_8009F270");
extern Node *gPendingCvkbNode __asm__("D_8009F274");
extern Node *gCshdNodeSlots[2] __asm__("D_8009F640");

extern void gFreeBlock(Node *node) __asm__("func_800181D4");
extern void func_800236FC(void);
void gRegisterCvkChunk(Node *node) __asm__("func_80023930");


/**
 *  @brief Registers or rejects a tagged resource node.
 *
 *  When registration is enabled, recognized tags select dedicated pointers
 *  or one of two slots for nodes whose first data halfword equals four.
 *  @param node Tagged node to register or pass to the rejection helper.
 *  @par Globals
 *  D_8009F26C enables registration; D_8009F270 and D_8009F274 hold tagged nodes.
 *  D_8009F640 holds up to two qualifying nodes of the third recognized tag.
 *  @par Control flow
 *  Dispatches by tag, scans two slots when needed, and rejects unsupported nodes.
 *  The enabled path always invokes the follow-up helper after dispatch.
 *  @par Calls
 *  func_800181D4 handles rejected nodes; func_800236FC performs enabled-path follow-up.
 *  @par Side effects
 *  May replace a dedicated pointer, fill a slot, or invoke either helper.
 *  @par Callers
 *  Confirmed: func_80024EC8.
 *
 * @par Environment path
 * Registers asset chunks routed here by gLoadQueuedAssets (func_80024EC8,
 * 80024EC8.c:185-188): a 'Cvkh' (0x43766B68) node becomes the pending
 * D_8009F270, a 'Cvkb' (0x43766B62) node the pending D_8009F274, and a 'Cshd'
 * (0x43736864) node whose first data half-word is 4 goes into the first free
 * of the two slots D_8009F640; anything else (or a third 'Cshd') is freed.
 * After each accepted or rejected node it calls func_800236FC, which, once
 * both pending nodes exist with equal ids, starts an entry in one of eight
 * slots via func_80021CB0 with a 1500-tick deadline and otherwise frees the
 * pair. When the subsystem flag D_8009F26C (set by func_8002333C, cleared by
 * func_80023478) is 0, every node is freed immediately.
 */
void gRegisterCvkChunk(Node *node)
{
    s32 i;
    register Node **p __asm__("$3");

    if (gCvkSystemActive != 0) {
        switch (node->tag) {
        case 0x43766B68:
            gPendingCvkhNode = node;
            break;

        case 0x43766B62:
            gPendingCvkbNode = node;
            break;

        case 0x43736864:
            if (*node->data != 4) {
                gFreeBlock(node);
                break;
            }

            i = 0;
            p = gCshdNodeSlots;
            for (; i < 2; i++, p++) {
                if (*p == 0) {
                    *p = node;
                    break;
                }
            }
            if (i == 2) {
                gFreeBlock(node);
            }
            break;

        default:
            gFreeBlock(node);
            break;
        }
        func_800236FC();
    } else {
        gFreeBlock(node);
    }
}
