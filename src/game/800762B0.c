// Categories: Gameplay > Object Spawning | Graphics > Renderer > Sprite Node
#include "game/types.h"

typedef struct {
    u8 pad00[0x34];
    u16 a34;
    s16 a36;
    u8 colors[9];
    u8 a41;
    s16 a42, a44, a46, a48, a4a, a4c, a4e, a50, a52, a54;
    u16 a56;
} Src;

typedef struct {
    u8 pad00[8];
    s16 a08, a0a;
    u8 pad0c[6];
    u16 a12;
    u8 pad14[0x10];
    s32 a24;
    u8 pad28[0x3c];
    u8 pool[12];
    s16 a70;
    u8 a72;
    u8 colors[3][3];
    s16 slopes[3][3];
    u16 a8e;
    s16 a90, a92, a94, a96, a98, a9a, a9c, a9e;
} Dest;

typedef struct {
    u8 pad[0x18];
    s32 x, y, z;
} Work;

extern u8 gAttachMessageName[] __asm__("D_80075698");
extern void *gAllocMemory(s32, s32) __asm__("func_80017EBC");
extern void gFreeBlock(void *) __asm__("func_800181D4");
extern u8 func_8003BE2C(void *, s32, s32);
extern void gInitSpawnMotionState(void *, void *) __asm__("func_8003C14C");
extern u32 func_80019668(void);
extern void gBuildRotationMatrix(s32, s32, s32, Work *) __asm__("func_800188D0");
void *gCreateEmitterObject(Src *arg) __asm__("func_800762B0");


/**
 * @brief Constructor of spawn kind 0x57: allocates the 0xA0 object and its
 *        node pool, copies the shared spawn header and turns the record's
 *        three colour key frames into a ramp with per-frame slopes.
 *
 * @param arg Spawn record. +0x00..+0x33 are read by func_8003C14C, +0x34 a
 *        u16 size, +0x36 a s16 rate numerator, +0x38..+0x40 nine colour bytes
 *        (three steps of R,G,B), +0x41 a mode byte, +0x42/+0x44/+0x46 the
 *        three segment times, +0x48 a limit, +0x4A/+0x4C the yaw and pitch
 *        spread (negative means "pick a random value in that range"), +0x4E
 *        the spread radius, +0x50/+0x52/+0x54 flag inputs and +0x56 the flags.
 * @return The new object, or 0 when the 0xA0 block or the 0x30-by-0x28 node
 *         pool could not be allocated (the block is released again in that
 *         case).
 *
 * @par Confirmed
 * func_80017EBC(0xA0, 1) allocates the object and func_8003BE2C(dest +0x64,
 * 0x30, 0x28) builds its 40-node pool; a non-1 result frees the object with
 * func_800181D4 and returns 0. func_8003C14C(dest, arg) then copies the common
 * spawn header.
 * The class message handler is installed at +0x24 (D_80075698), the mode byte
 * at +0x72 = arg +0x41, the size at +0x12 = arg +0x34, and +0x90/+0x94/+0x96/
 * +0x98 = arg +0x48/+0x50/+0x52/+0x54.
 * Yaw = arg +0x4A and pitch = arg +0x4C; a negative value is replaced by
 * func_80019668() % (-value). func_800188D0(yaw, -pitch, 0, &work) writes the
 * 3x3 matrix over the local block and its third row (read back at +0x18..+0x24)
 * becomes the stored vector: +0x9A/+0x9C/+0x9E = ((row >> 4) * arg +0x4E) >> 12.
 * The nine colour bytes are transposed into +0x73 + channel * 3 + step, i.e.
 * dest->colors[channel][step] with channel 0..2 and step 0..2.
 * For each step: a time below 16 zeroes the three slopes; step 2 gets
 * (-colour << 12) / time, the others ((next - current) << 12) / time, per
 * channel, into +0x7C + channel * 6 + step * 2.
 * +0x70 = sum of the three times, replaced by (arg +0x36 << 12) / sum when the
 * sum is non-zero.
 * Flags: +0x8E = arg +0x56 with 0x8000 set when any of arg +0x4E/+0x50/+0x52 is
 * non-zero, and 0x4000 set when arg +0x48 < (arg +0x4E * sum) >> 12. Finally
 * +0x0A = 10 and +0x08 = 0.
 *
 * @par Likely role
 * The spawn factory of the light/flare/sprite class whose per-frame behaviour
 * lives in D_80075698 and func_800754B8: the three colour steps are animation
 * key frames, the slopes the fixed-point deltas between them and +0x70 a
 * per-frame rate, while 0x8000 gates the anchor and rotation work in
 * func_800754B8. The random yaw/pitch pair gives every instance a different
 * orientation of its +0x9A offset.
 *
 * @par Unknown
 * What the untouched members +0x08 (0) and +0x0A (10) drive, whether the slopes
 * are applied per frame or per tick, and what distinguishes the two flag inputs
 * arg +0x50/+0x52 that only feed the same 0x8000 bit as the radius.
 *
 * @par Callers
 * func_8003E984, kind 0x57 of its per-kind constructor switch.
 *
 * @par Environment path
 * Constructor for spawn kind 0x57, called only from gSpawnObjectFromTable
 * (func_8003E984): it allocates a 0xA0-byte object with gAllocMemory (tag 1),
 * builds a pool of 0x28 particle nodes of 0x30 bytes at +0x64 with
 * func_8003BE2C (freeing the object and returning 0 on failure), copies the
 * common spawn header with func_8003C14C and stores the handler address
 * 0x80075698 at +0x24. From the record it derives a three-step RGB colour
 * ramp with 4.12 fixed-point slopes per step (+0x73/+0x7C), the emission rate
 * at +0x70, flags at +0x8E (0x8000 when a spread radius or flag input is set,
 * 0x4000 when the limit +0x48 is below radius * total time) and an offset
 * vector at +0x9A rotated by a yaw/pitch that is randomised when negative
 * (gBuildRotationMatrix, func_800188D0). The particles are later created by
 * gInitEmitterParticle (func_800754B8) and stepped by gRunPolygonPass
 * (func_80075A84), which reads the same +0x8E flags.
 */
void *gCreateEmitterObject(Src *arg)
{
    register Src *src __asm__("$17");
    Dest *dest;
    s16 time[3];
    Work work;
    register s32 x __asm__("$18");
    register s32 angle __asm__("$4");
    register s32 second __asm__("$2");
    register s32 y __asm__("$5");
    s32 i;
    register u32 flags __asm__("$5");
    register s32 a50 __asm__("$4");
    register s32 a52 __asm__("$3");
    register s32 a4e __asm__("$2");

    src = arg;
    dest = gAllocMemory(0xA0, 1);
    if (!dest) return 0;
    if (func_8003BE2C(dest->pool, 0x30, 0x28) != 1) {
        gFreeBlock(dest);
        return 0;
    }
    gInitSpawnMotionState(dest, src);
    dest->a24 = (s32)gAttachMessageName;
    dest->a72 = src->a41;
    dest->a12 = src->a34;
    dest->a90 = src->a48;
    dest->a94 = src->a50;
    dest->a96 = src->a52;
    dest->a98 = src->a54;
    angle = src->a4a;
    x = angle;
    if (angle < 0) x = func_80019668() % -src->a4a;
    second = src->a4c;
    if (second < 0) {
        y = func_80019668() % -src->a4c;
        angle = x;
    } else {
        y = second;
        angle = x;
    }
    gBuildRotationMatrix(angle, -y, 0, &work);
    dest->a9a = ((work.x >> 4) * src->a4e) >> 12;
    dest->a9c = ((work.y >> 4) * src->a4e) >> 12;
    dest->a9e = ((work.z >> 4) * src->a4e) >> 12;
    time[0] = src->a42;
    time[1] = src->a44;
    time[2] = src->a46;
    dest->colors[0][0] = src->colors[0];
    dest->colors[1][0] = src->colors[1];
    dest->colors[2][0] = src->colors[2];
    dest->colors[0][1] = src->colors[3];
    dest->colors[1][1] = src->colors[4];
    dest->colors[2][1] = src->colors[5];
    dest->colors[0][2] = src->colors[6];
    dest->colors[1][2] = src->colors[7];
    dest->colors[2][2] = src->colors[8];
    for (i = 0; i < 3; i++) {
        if (time[i] < 16) {
            dest->slopes[0][i] = 0;
            dest->slopes[1][i] = 0;
            dest->slopes[2][i] = 0;
        } else if (i == 2) {
            dest->slopes[0][2] = (-dest->colors[0][2] << 12) / time[2];
            dest->slopes[1][2] = (-dest->colors[1][2] << 12) / time[2];
            dest->slopes[2][2] = (-dest->colors[2][2] << 12) / time[2];
        } else {
            dest->slopes[0][i] = ((dest->colors[0][i+1] - dest->colors[0][i]) << 12) / time[i];
            dest->slopes[1][i] = ((dest->colors[1][i+1] - dest->colors[1][i]) << 12) / time[i];
            dest->slopes[2][i] = ((dest->colors[2][i+1] - dest->colors[2][i]) << 12) / time[i];
        }
    }
    dest->a70 = time[0] + time[1] + time[2];
    if (dest->a70 != 0) dest->a70 = (src->a36 << 12) / dest->a70;
    flags = src->a56;
    a50 = src->a50;
    a4e = src->a4e;
    a52 = src->a52;
    if (a4e != 0) {
        a4e = a52 | 1;
        a4e = a50 | a4e;
        if (a4e) goto flag_set;
        dest->a8e = flags;
        goto flag_done;
    }
    a4e = a50 | a52;
    if (a4e) goto flag_set;
    goto flag_store;
flag_set:
    flags |= 0x8000;
flag_store:
    dest->a8e = flags;
flag_done:
    if (src->a48 < ((src->a4e * (time[0] + time[1] + time[2])) >> 12)) {
        a4e = flags | 0x4000;
        dest->a8e = a4e;
    }
    dest->a0a = 10;
    dest->a08 = 0;
    return dest;
}
