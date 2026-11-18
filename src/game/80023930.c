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

extern u8 gChannelEnableFlag __asm__("D_8009F26C");
extern Node *D_8009F270;
extern Node *D_8009F274;
extern Node *gStreamStatePair[2] __asm__("D_8009F640");

extern void gFreeBlock(Node *node) __asm__("func_800181D4");
extern void func_800236FC(void);

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
 */
void func_80023930(Node *node)
{
    s32 i;
    register Node **p __asm__("$3");

    if (gChannelEnableFlag != 0) {
        switch (node->tag) {
        case 0x43766B68:
            D_8009F270 = node;
            break;

        case 0x43766B62:
            D_8009F274 = node;
            break;

        case 0x43736864:
            if (*node->data != 4) {
                gFreeBlock(node);
                break;
            }

            i = 0;
            p = gStreamStatePair;
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
