// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

struct Vec { s32 x, y, z; };
struct State { u8 pad[0x34]; u8 *frames; };
struct Object {
    u8 pad00[0x0c];
    u8 command;
    u8 pad0d[0x17];
    u32 flags;
    struct State *state;
    u8 matrix[0x24];
    struct Vec position;
    struct Vec output;
    u8 pad68[8];
    s16 frame;
    u8 pad72[0x2e];
    u8 anim[0x3c];
    u32 animflags;
    u8 pade0[0x3c];
    u8 bytes[4];
};
struct Part { u8 pad[0x24]; u8 data[0x14]; u8 bytes[4]; };
struct Info { u8 pad[0x18]; s32 value; };
struct Other { u8 pad[4]; struct Info *info; struct Part *part; };
extern s32 func_8002AD58(s32, struct Object *, struct Other *);
extern void func_80050E0C(void *, void *, s32, void *, void *);
extern s32 func_80050D14(void *, struct Other *, struct Object *);
extern void func_80039BD0(struct Object *);
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern struct Object *gMissionWorkingSet __asm__("D_8009F9A0");
#define current gMissionWorkingSet
#define barrier() __asm__ volatile("" : : : "memory")

/**
 * @brief Entity message handler of the third script family, with a "current"
 *        pointer in D_8009F9A0.
 *
 * @param message Message id.
 * @param self Object: anim at +0x34, matrix, output, command, animflags, bytes.
 * @param other Partner whose info->value is passed to the part update.
 * @return 0x120 for message 1, 0 for 2/3/4/5/10/11/13/14 paths, otherwise the
 *         delegated result of func_8002AD58.
 *
 * @par Confirmed
 * func_8002AD58(message, self, other) runs the behaviour; the part update is
 * func_80050E0C(part->data, self->anim, other->info->value, self, &self->position),
 * then func_80010750 + func_80010234 fetch one frame vertex into self->output.
 * Message 5 clears the global current pointer when it is this object, message 6
 * calls func_80039BD0 and func_80050D14 and refreshes the matrix of the state.
 *
 * @par Likely role
 * The handler of the "active/attached" entity family: D_8009F9A0 holds the one
 * object that is currently being driven, and messages 5/6 take or drop that role.
 */
s32 func_80039C58(s32 message, struct Object *self, struct Other *other)
{
    struct Part *part;
    register s32 offset __asm__("$4");
    register u8 *pointer __asm__("$2");
    register s32 result __asm__("$2");
    switch (message) {
    case 1:
        return 0x120;
    case 2:
        func_8002AD58(message, self, other);
        part = other->part;
        self->bytes[0] = part->bytes[0];
        self->bytes[1] = part->bytes[1];
        self->bytes[2] = part->bytes[2];
        self->bytes[3] = part->bytes[3];
        func_80050E0C(part->data, self->anim, other->info->value, self, &self->position);
        self->flags &= 0x7fffffff;
        if (self->state) {
            gGteLoadMatrix(self->matrix);
            offset = self->frame;
            pointer = (u8 *)self->state;
            offset <<= 4;
            __asm__("" : "=r"(offset) : "0"(offset));
            pointer = ((struct State *)pointer)->frames;
            offset += 8;
            gGteTransformVerticesWide(pointer + offset, &self->output, 1);
        } else {
            self->output = self->position;
        }
    set_current:
        if (!(self->animflags & 0x40)) { current = self; barrier(); }
        return 0;
    case 6:
        if (self->command) func_80039BD0(self);
        if (func_80050D14(self->anim, other, self)) {
            if (self->state) {
                gGteLoadMatrix(self->matrix);
                offset = self->frame;
            pointer = (u8 *)self->state;
            offset <<= 4;
            __asm__("" : "=r"(offset) : "0"(offset));
            pointer = ((struct State *)pointer)->frames;
            offset += 8;
            gGteTransformVerticesWide(pointer + offset, &self->output, 1);
            } else {
                self->output = self->position;
            }
        }
        if (current != self) goto set_current;
        result = self->animflags & 0x40;
        if (!result) goto zero_return;
        result = 0;
        current = 0;
        __asm__ volatile("" : "+r"(result) : : "memory");
        return result;
    case 5:
        if (current == self) { current = 0; barrier(); }
        return func_8002AD58(message, self, other);
    case 3: case 4: case 10: case 11: case 13: case 14:
    zero_return:
        barrier();
        return 0;
    default:
        return func_8002AD58(message, self, other);
    }
}
