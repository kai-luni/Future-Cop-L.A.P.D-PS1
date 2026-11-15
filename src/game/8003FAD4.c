// Categories: Game Engine > Screen Teardown | Graphics > Scene List | Graphics > Display Setup
#include "game/types.h"

struct Unk3BObject;

typedef struct UnkFAD4Node {
    struct UnkFAD4Node *next;
    struct Unk3BObject *object;
} UnkFAD4Node;

typedef struct UnkFAD4Local {
    u32 color[2];
    s16 rect[4];
} UnkFAD4Local;

extern UnkFAD4Node *D_8009F708;
extern s32 D_8009F70C;
extern s32 D_8009F9B8;
extern s32 D_8009FAA0;

extern void func_8003C590(s32 arg0);
extern void func_8003BC0C(struct Unk3BObject *object);
extern void func_80019584(s32 arg0);
extern void func_80092F38(s16 *rect, u32 *color);
extern void func_80092C8C(s32 arg0);

/**
 * @brief Clears the screen's scene: drops the two render lists, releases every
 *        object of the per-screen list and paints the backdrop colour.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. func_8003C590(0) and func_8003C590(1) - reset/drop the two render lists
 *    (buffer index 0 and 1).
 * 2. Walk the singly linked list D_8009F708 ({next, object}) and call
 *    func_8003BC0C(object) per node - the objects registered for this screen
 *    are released/closed.
 * 3. func_80019584(D_8009F70C) releases the list anchor block.
 * 4. D_8009F9B8 = 0x13E, D_8009FAA0 = 0x15E (two colour/limit values used by
 *    the renderer), D_8009F70C = 0 and D_8009F708 = 0 clear the list state.
 * 5. A rectangle (0x380, 0x100, 2, 2) is emitted with two colour words
 *    (0xC210C210 each) through func_80092F38 and the GPU is synchronised with
 *    func_80092C8C(0).
 *
 * @par Globals
 * D_8009F708 (object list head), D_8009F70C (list anchor), D_8009F9B8,
 * D_8009FAA0 (renderer limits/colours).
 *
 * @par Likely role
 * The end-of-screen cleanup that the front-end runs right after the module has
 * been started (func_800287F4 calls it unconditionally), i.e. "forget the
 * previous screen's scene before the new one fills the lists".
 *
 * @par Unknown
 * The exact purpose of the 2x2 rectangle and of the 0xC210C210 colour constant
 * (the same value twice suggests a 16-bit pair, e.g. a fill and a clear value).
 */
void func_8003FAD4(void)
{
    UnkFAD4Node *node = D_8009F708;
    UnkFAD4Local local;

    func_8003C590(0);
    func_8003C590(1);

    while (node != 0) {
        func_8003BC0C(node->object);
        node = node->next;
    }

    func_80019584(D_8009F70C);

    local.color[0] = 0xC210C210;
    local.color[1] = 0xC210C210;
    D_8009F9B8 = 0x13E;
    D_8009FAA0 = 0x15E;
    local.rect[0] = 0x380;
    local.rect[1] = 0x100;
    D_8009F70C = 0;
    D_8009F708 = 0;
    local.rect[2] = 2;
    local.rect[3] = 2;

    func_80092F38(local.rect, local.color);
    func_80092C8C(0);
}
