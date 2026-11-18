// Categories: Graphics > Scene Graph | Game Engine > Object Pool
#include "game/types.h"

struct Unk17994Block {
    s32 words[12];
};

struct Unk17994Node {
    u8 unk00[0x28];
    struct Unk17994Block first;
    struct Unk17994Block second;
    s32 id;
    struct Unk17994Node *next;
    struct Unk17994Node *previous;
};

extern struct Unk17994Block gBaseMatrix __asm__("D_8009AA38");
extern struct Unk17994Node *gTransformRecordList __asm__("D_8009F588");
extern s32 gTransformSerialCounter __asm__("D_8009F58C");

extern void *func_80017EBC(s32 size, s32 tag);

/**
 * @brief Allocates a node with two copies of the neutral matrix template.
 *
 * A 0x94-byte block of type 2 is allocated and both 12-word blocks that follow
 * the 0x28-byte header are filled from D_8009AA38, which makes them two
 * identical 3x3 matrices in 16.16 fixed point. The node gets the next id from
 * D_8009F58C and is appended to the list whose head is D_8009F588, with the
 * previous pointer set to the current tail.
 *
 * @return The id assigned to the new node.
 *
 * @par Globals
 * D_8009AA38 - matrix template; D_8009F588 - list head; D_8009F58C - id counter.
 *
 * @par Control flow
 * Straight-line copy, then an append with a tail walk.
 *
 * @par Calls
 * func_80017EBC - upper allocator.
 *
 * @par Side effects
 * Grows the node list and increments the id counter.
 *
 * @par Callers
 * Confirmed: func_800573A4.
 *
 * @par Environment path
 * Allocates a 0x94-byte record with func_80017EBC and fills both of its
 * 12-word matrix blocks from the neutral 16.16 matrix template D_8009AA38,
 * then stamps the node with the next value of the serial counter D_8009F58C
 * and appends it to the doubly linked list at D_8009F588 (tail walk, next and
 * previous set). It returns the new id. It draws nothing and is not part of
 * the render path itself, but its only caller func_800573A4:268 stores that
 * id in the actor and immediately passes it to gSelectViewRecord
 * (func_80017B60), so the record is the per-actor node whose id selects the
 * map/camera view record.
 */
s32 func_80017994(void)
{
    register struct Unk17994Node *node __asm__("$5");
    register struct Unk17994Node *tail __asm__("$6");
    register struct Unk17994Block *template __asm__("$3");
    register s32 firstWord __asm__("$4");

    node = func_80017EBC(0x94, 2);
    template = &gBaseMatrix;
    firstWord = template->words[0];
    node->first.words[0] = firstWord;
    node->first.words[1] = template->words[1];
    node->first.words[2] = template->words[2];
    node->first.words[3] = template->words[3];
    node->first.words[4] = template->words[4];
    node->first.words[5] = template->words[5];
    node->first.words[6] = template->words[6];
    node->first.words[7] = template->words[7];
    node->first.words[8] = template->words[8];
    node->first.words[9] = template->words[9];
    node->first.words[10] = template->words[10];
    node->first.words[11] = template->words[11];
    node->second.words[0] = firstWord;
    node->second.words[1] = template->words[1];
    node->second.words[2] = template->words[2];
    node->second.words[3] = template->words[3];
    node->second.words[4] = template->words[4];
    node->second.words[5] = template->words[5];
    node->second.words[6] = template->words[6];
    node->second.words[7] = template->words[7];
    node->second.words[8] = template->words[8];
    node->second.words[9] = template->words[9];
    node->second.words[10] = template->words[10];
    node->second.words[11] = template->words[11];
    node->id = gTransformSerialCounter++;
    node->next = 0;
    node->previous = 0;

    if (gTransformRecordList != 0) {
        tail = gTransformRecordList;
        while (tail->next != 0) {
            tail = tail->next;
        }
        tail->next = node;
        node->previous = tail;
    } else {
        gTransformRecordList = node;
    }
    return node->id;
}
