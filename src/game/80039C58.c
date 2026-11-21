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
extern s32 gEntityBaseHandler(s32, struct Object *, struct Other *) __asm__("func_8002AD58");
extern void gInitRailSegment(void *, void *, s32, void *, void *) __asm__("func_80050E0C");
extern s32 gUpdatePathMoverMotion(void *, struct Other *, struct Object *) __asm__("func_80050D14");
extern void gApplyCameraOverrideOpcode73(struct Object *) __asm__("func_80039BD0");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern struct Object *gFocusPositions __asm__("D_8009F9A0");
#define current gFocusPositions
#define barrier() __asm__ volatile("" : : : "memory")
s32 gHandleCameraOverrideMsg(s32 message, struct Object *self, struct Other *other) __asm__("func_80039C58");


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
 *
 * @par Environment path
 * Message handler of the camera-override object class (size 0x120 on message
 * 1); no direct call site exists, it is reached through the object handler
 * table. Message 2 runs the generic setup func_8002AD58, copies four control
 * bytes from the spawn part into +0x11C..+0x11F, starts the path motion with
 * func_80050E0C and refreshes the reference vertex; message 6 drains queued
 * 0x73 commands (func_80039BD0), steps the path with func_80050D14 and
 * refreshes the vertex again. Both make the object the global override
 * D_8009F9A0 unless its flag 0x40 is set (message 6 releases it when the flag
 * is set), and message 5 clears the global before delegating to
 * func_8002AD58. While D_8009F9A0 is set, the chase camera func_800569EC
 * takes its heading, state and position, and func_8002703C can force the
 * frame step from byte +0x11D.
 */
s32 gHandleCameraOverrideMsg(s32 message, struct Object *self, struct Other *other)
{
    struct Part *part;
    register s32 offset __asm__("$4");
    register u8 *pointer __asm__("$2");
    register s32 result __asm__("$2");
    switch (message) {
    case 1:
        return 0x120;
    case 2:
        gEntityBaseHandler(message, self, other);
        part = other->part;
        self->bytes[0] = part->bytes[0];
        self->bytes[1] = part->bytes[1];
        self->bytes[2] = part->bytes[2];
        self->bytes[3] = part->bytes[3];
        gInitRailSegment(part->data, self->anim, other->info->value, self, &self->position);
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
        if (self->command) gApplyCameraOverrideOpcode73(self);
        if (gUpdatePathMoverMotion(self->anim, other, self)) {
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
        return gEntityBaseHandler(message, self, other);
    case 3: case 4: case 10: case 11: case 13: case 14:
    zero_return:
        barrier();
        return 0;
    default:
        return gEntityBaseHandler(message, self, other);
    }
}
