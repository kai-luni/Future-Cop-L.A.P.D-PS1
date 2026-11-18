// Categories: Graphics > Camera > Chase Rig | Gameplay > Player | Graphics > Camera > Smoothing
#include "game/types.h"

struct Vec569EC { s32 x, y, z; };
struct Info569EC {
    u8 pad00[0x30];
    void *model;
    u8 pad34[0x10];
    struct Vec569EC position;
    u8 pad50[0x50];
    u16 lock;
    s16 effect;
    u8 state;
    u8 padA5;
    s8 current;
    s8 next;
    s16 zoom;
    s16 timer;
    s8 camera;
    u8 padAD;
    s16 cooldown;
    struct Vec569EC offset;
    s32 distance;
    s8 mode;
    u8 padC1[0xF];
    s16 shake;
    u8 padD2[0x23E];
    s8 restart;
    s8 selected;
};
struct Obj569EC {
    u8 pad00[0x50];
    struct Vec569EC position;
    struct Vec569EC previous;
    void *collision;
    u8 pad6C[0x40];
    struct Info569EC *info;
    s16 heading;
};
struct Override569EC {
    u8 pad00[0x50];
    struct Vec569EC position;
    u8 pad5C[0x92];
    s16 heading;
    u8 padF0[0x2E];
    s8 state;
};
struct Mode569EC { u8 pad00[0x5C]; s32 mode; };
struct Camera569EC {
    u8 pad00[0x1C];
    s32 pitch;
    s32 yaw;
    u8 pad24[0x34];
    struct Vec569EC axis58;
    struct Vec569EC axis64;
    struct Vec569EC axis70;
};
struct Offset569EC {
    s16 yaw;
    s16 pitch;
    s32 distance;
    struct Vec569EC offset;
};
struct State569EC { s8 *indices; u32 flags; };
struct Locals569EC {
    struct Vec569EC out;
    s32 pad0C;
    struct Vec569EC delta;
    s32 pad1C;
    struct Vec569EC position;
    s32 pad2C;
    struct Vec569EC previous;
    s32 pad3C;
    struct Camera569EC *saved;
};

extern struct Mode569EC *gGameSession __asm__("D_8009FA94");
extern struct Override569EC *gMissionWorkingSet __asm__("D_8009F9A0");
extern s32 gModuleLoadedFlag __asm__("D_8009FA84");
extern struct Camera569EC *gCamera __asm__("D_8009F9E4");
extern struct State569EC gPresetEntryTable[] __asm__("D_8009AEC4");
extern struct Offset569EC gViewPresetTable[] __asm__("D_8009AD08");
extern s32 gScreenState __asm__("D_8009FA00");
extern void func_8005780C(void *, struct Vec569EC *);
extern s32 func_80031B40(struct Vec569EC *, void *);
extern s32 func_80056530(struct Obj569EC *, s32, s32);
extern s32 func_80056870(struct Obj569EC *);
extern u8 func_80056994(struct Info569EC *, s32);
extern void func_80017B60(s32);
extern void gPlaceCameraFromPreset(struct Obj569EC *, s32, struct Vec569EC *, s32) __asm__("func_8005678C");
extern void func_8005612C(struct Vec569EC *, s32, s32, s32, s32, struct Vec569EC *, struct Vec569EC *, s32, s32);
extern s32 func_80055EE8(struct Vec569EC *, struct Vec569EC *);
extern s32 func_8005605C(s32, s32, s32);
extern s32 func_800564A8(s32);
extern s32 func_8001A314(struct Vec569EC *);
extern void func_8001AB78(s32, struct Vec569EC *);
extern void gSetCameraRotation(s32, s32, s32) __asm__("func_80017860");
extern void func_8001AC28(struct Vec569EC *, struct Vec569EC *, s32, struct Vec569EC *);
extern s32 func_80095C14(s32);
extern s32 func_80019668(void);
extern void func_8001EA20(struct Obj569EC *, s32);
extern void gSetCameraPosition(struct Vec569EC *) __asm__("func_800177AC");

#define ABS569EC(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

#define RELOAD_OBJECT() (*(struct Obj569EC *volatile *)&object)
void gUpdateChaseCamera(struct Obj569EC *object, volatile s32 elapsed, volatile s32 speed) __asm__("func_800569EC");


/**
 * @brief Chase-camera step of one player unit: selects the camera preset, probes
 *        it against the map, then smooths yaw, pitch, boom length and eye offset
 *        into the view record picked by the unit's camera id.
 *
 * @param object Unit object. +0xAC is the info/state block used below, +0x50 the
 *        current position, +0x5C the previous position, +0x68 the zone/collision
 *        handle, +0xB0 the heading.
 * @param elapsed Frame step, in the same tick unit as the game's timers.
 * @param speed Sub-tick count handed to the angle and offset smoothers (the caller
 *        passes the odd-step accumulator, i.e. roughly half of elapsed).
 * @return Nothing.
 *
 * @par Confirmed
 * Preset selection:
 * func_8005780C is first called with the unit's button word at info +0x30 and the
 * four-byte press/release phase record at info +0x44. A phase of 0 (the leading
 * edge) with the restart byte +0x310 clear advances the preset byte +0xA4, wrapping
 * from 7 to 0, and sets current +0xA6 = next +0xA7 = 1, timer +0xAA = 0 and
 * cooldown +0xAE = 900. In presets 5 and 6 a failing func_80031B40(&object +0x50,
 * object +0x68) - the map zone test over the nodes flagged 0x40 - resets +0xA4 to 0.
 * The saved position and previous position are copied into a local work block and
 * only the y component is replaced, by func_80056530(object, elapsed, speed), the
 * vertical smoother that walks the state word at +0xD4 of the object's +0xAC block
 * toward the target at object +0x54.
 *
 * View source:
 * The selected slot is held in +0x311: 17 while the lock word +0xA0 is non-zero,
 * otherwise it is re-picked by func_80056870(object) (a random slot 9..16 that
 * passes func_80055EE8) when the mode byte +0xC0 is set and the game mode at
 * D_8009FA94 +0x5C is not 1, and cleared to 0 in all other cases. A non-null
 * D_8009F9A0 (scripted view override, heading +0xEE, index +0x11E) replaces heading,
 * position, previous position and the preset index (index - 1), and a positive
 * D_8009FA84 replaces the preset with func_80056994(info, elapsed), a per-side
 * sequence D_800A2290[side at +0xC1][step at +0x313] that advances every 3000 steps
 * through the countdown at +0x2FE.
 *
 * View update:
 * D_8009F9E4 (the camera record) is saved at entry and restored at exit, so the
 * record selected by func_80017B60(info +0xAC) is only current inside this function.
 * A positive selected slot places the view through func_8005678C(object, slot, out,
 * 1) and skips the rest. Otherwise the preset entry D_8009AEC4[+0xA4] supplies a
 * signed index list (D_8009AD08 entries, 20 bytes each: yaw +0, pitch +2, distance
 * +4, eye offset +8) and a flag word at +4: 0x1 adds elapsed to the zoom +0xA8 and
 * clamps at 256, 0x8 clamps at 300, a clear pair subtracts and floors at 0; 0x2 forces
 * heading -0x400; 0x4 runs the blocked-view search, which tests the current slot
 * through func_8005612C and the map probe func_80055EE8, accumulates +0xAA while
 * blocked or while +0xA6 is not 1, and past 120 tries index[1] and then walks the
 * list (+0xA7, wrapping to 2 on a negative entry), accepting the first free
 * candidate or parking +0xAA at 120.
 *
 * Smoothing:
 * yaw = func_8005605C(camera->yaw, heading + entry yaw, speed) and
 * pitch = func_8005605C(camera->pitch, entry pitch, speed), each limited to
 * elapsed * 4 (yaw) / elapsed * 2 (pitch) units per step, with the yaw unwrapped
 * across the 0x800 boundary; the boom length comes from func_800564A8(pitch)
 * (0x10000 - ((pitch - 0x14A) << 16) / 600, squared, doubled, + 0xD800, floored
 * at 0x9000). The +0xB0 eye offset is lerped toward the preset offset at
 * elapsed * 0x50 per step, snapping when the remaining length is shorter
 * (func_8001A314 / func_8001AB78). func_8005612C then places the view,
 * func_80017860(yaw, pitch, 0) writes the camera matrix, func_8001AC28 shifts the
 * eye along camera +0x70 by zoom << 7, and a non-zero shake +0xD0 (capped at 255)
 * becomes a power-of-two mask through func_80095C14 and jitters the view by
 * (random & (mask - 1)) - mask / 2 along the camera axes +0x64 and +0x58 before
 * decaying by elapsed. The effect word +0xA2 counts down by 2 * elapsed and drives
 * func_8001EA20(object, value + 90) in screen state 4 (D_8009FA00 == 4), else
 * func_8001EA20(object, 0); func_800177AC(out) posts the position last.
 *
 * @par Likely role
 * The chase camera of a player vehicle: the preset byte is one of seven camera
 * modes cycled with the unit's camera button, the cooldown byte suppresses
 * re-triggering, and the preset lists in D_8009AEC4/D_8009AD08 are the seven
 * boom/pitch/length combinations, including two that are only valid outside
 * map zones and one that looks backwards.
 *
 * @par Unknown
 * What the value 17 in the selected slot means (a "no preset" marker for other
 * code), the units of the 20-byte offset records, and whether D_8009FA84 really is
 * the attract/demo countdown described by func_80027510.
 *
 * @par Callers
 * func_800588D4, the message 6 handler of the unit class, which forwards its own
 * elapsed and odd-step accumulator; its documentation names this function as the
 * camera step.
 *
 * @par Environment path
 * Chase-camera step: selects the preset, probes it against the map and
 * smooths yaw, pitch, boom and eye offset into the view record selected by
 * the unit's camera id - the camera the environment is drawn from.
 */
void gUpdateChaseCamera(struct Obj569EC *object, volatile s32 elapsed, volatile s32 speed)
{
    struct Locals569EC local;
    struct Info569EC *info;
    register struct Obj569EC *obj __asm__("$8");
    register s32 tick __asm__("$8");
    struct Offset569EC *offset;
    s8 *indices;
    s32 state;
    s32 selected;
    register s32 heading __asm__("$21");
    register s32 blocked __asm__("$18");
    register s32 yaw __asm__("$17");
    register s32 pitch __asm__("$18");
    register s32 distance __asm__("$22");
    register s32 rate __asm__("$4");
    register s32 old __asm__("$6");
    s32 difference;
    s32 shake;
    register s32 mask __asm__("$17");
    register s32 half __asm__("$16");
    u32 flags;
    u32 value;
    s32 length;
    register struct Vec569EC *deltaPtr __asm__("$20");
    register s32 scaled __asm__("$3");
    struct Vec569EC *posPtr;

    func_8005780C(object->info->model, &object->info->position);
    obj = RELOAD_OBJECT();
    __asm__("" : "+r"(obj));
    info = obj->info;
    if (info->position.y == 0 && info->restart == 0) {
        info->state++;
        if (info->state == 7) info->state = 0;
        info->next = 1;
        info->current = 1;
        info->timer = 0;
        info->cooldown = 900;
    }
    if ((u32)(info->state - 5) < 2) {
        obj = RELOAD_OBJECT();
    __asm__("" : "+r"(obj));
        if (func_80031B40(&obj->position, obj->collision) == 0) {
            info->state = 0;
        }
    }
    obj = RELOAD_OBJECT();
    __asm__("" : "+r"(obj));
    state = info->state;
    heading = obj->heading;
    local.position = obj->position;
    local.previous = obj->previous;
    __asm__("" ::: "memory");
    {
        register struct Obj569EC *callObject __asm__("$4");
        register s32 callElapsed __asm__("$5");
        register s32 callSpeed __asm__("$6");
        callObject = RELOAD_OBJECT();
        callElapsed = elapsed;
        callSpeed = speed;
        local.position.y = func_80056530(callObject, callElapsed, callSpeed);
    }
    if (info->lock != 0) {
        info->selected = 17;
    } else if (info->mode != 0 && gGameSession->mode != 1) {
        if (info->selected == 0) {
            info->selected = func_80056870(object);
        }
    } else {
        if (info->selected != 0) info->selected = 0;
    }
    {
        register struct Override569EC *override __asm__("$7");
        register s32 overrideIndex __asm__("$2");
        override = gMissionWorkingSet;
        selected = 0;
        if (override != 0) {
            heading = override->heading;
            overrideIndex = override->state;
            local.position = override->position;
            local.previous = local.position;
            state = overrideIndex - 1;
        } else {
            selected = info->selected;
        }
    }
    if (gModuleLoadedFlag > 0) state = func_80056994(info, elapsed);
    {
        register struct Camera569EC *saved __asm__("$8");
        register s32 id __asm__("$4");
        saved = gCamera;
        id = info->camera;
        local.saved = saved;
        func_80017B60(id);
    }
    if (selected > 0) {
        gPlaceCameraFromPreset(object, selected, &local.out, 1);
        goto done;
    }
    {
        register struct State569EC *base __asm__("$2");
        register struct State569EC *path __asm__("$3");
        __asm__("lui\t%0,0x800A" : "=r"(base));
        base = (struct State569EC *)((s32)base - 0x513C);
        path = (struct State569EC *)((state << 3) + (s32)base);
        __asm__("" : "+r"(path));
        flags = path->flags;
        indices = path->indices;
    }
    if (flags & 9) {
        register s32 low __asm__("$2");
        register s32 sum __asm__("$3");
        low = (u16)info->zoom;
        __asm__("" : "+r"(low) : : "memory");
        tick = elapsed;
        sum = low + tick;
        info->zoom = sum;
        if (flags & 1) {
            register s32 signedSum __asm__("$2");
            signedSum = (s16)sum;
            __asm__("" : "+r"(signedSum));
            if (signedSum > 256) info->zoom = 256;
        } else {
            register s32 signedSum __asm__("$2");
            signedSum = (s16)sum;
            __asm__("" : "+r"(signedSum));
            if (signedSum > 300) info->zoom = 300;
        }
    } else {
        register s32 low __asm__("$2");
        low = (u16)info->zoom;
        __asm__("" : "+r"(low) : : "memory");
        tick = elapsed;
        low = low - tick;
        info->zoom = low;
        if ((s16)low < 0) info->zoom = 0;
    }
    {
        register s32 signedCooldown __asm__("$2");
        register s32 lowCooldown __asm__("$3");
        signedCooldown = info->cooldown;
        lowCooldown = (u16)info->cooldown;
        __asm__("" : "+r"(signedCooldown), "+r"(lowCooldown));
        if (signedCooldown > 0) {
            register s32 result __asm__("$2");
            tick = elapsed;
            __asm__("" : : "r"(tick));
            result = lowCooldown - tick;
            __asm__("" : "+r"(result) : "r"(tick));
            info->cooldown = result;
        }
    }
    if (flags & 2) heading = -1024;
    posPtr = &local.position;
    if (flags & 4) {
        register struct Vec569EC *testOut __asm__("$17");
        register struct Vec569EC *prevPtr __asm__("$23");
        register struct Vec569EC *argPosition __asm__("$4");
        argPosition = posPtr;
        {
            register struct Offset569EC *base __asm__("$8");
            register s32 index __asm__("$7");
            __asm__("lui\t%0,0x800A" : "=r"(base) : "r"(argPosition));
            base = (struct Offset569EC *)((s32)base - 0x52F8);
            index = indices[info->current];
            __asm__("" : : "r"(index) : "$17");
            testOut = &local.out;
            offset = (struct Offset569EC *)(index * 20 + (s32)base);
        }
        func_8005612C(argPosition, heading, heading + offset->yaw, offset->pitch,
                     offset->distance, &offset->offset, testOut, info->zoom, info->distance);
        prevPtr = &local.previous;
        blocked = func_80055EE8(prevPtr, testOut);
        if (blocked != 0 || info->current != 1) {
            value = (u16)info->timer;
            __asm__("" : : "r"(value) : "memory");
            tick = elapsed;
            {
                register s32 sum __asm__("$2");
                sum = value + tick;
                __asm__ __volatile__("" : "=r"(value) : "0"(sum));
                info->timer = value;
            }
        __asm__("" : : "r"(tick));
            if (info->timer > 120) {
                info->timer = 0;
                if (info->current != 1) {
                    {
                        register struct Offset569EC *base __asm__("$8");
                        register s32 index __asm__("$7");
                        argPosition = posPtr;
                        __asm__("lui\t%0,0x800A" : "=r"(base) : "r"(argPosition));
                        index = indices[1];
                        __asm__("" : "+r"(base) : "r"(index));
                        base = (struct Offset569EC *)((s32)base - 0x52F8);
                        offset = (struct Offset569EC *)(index * 20 + (s32)base);
                    }
                    func_8005612C(argPosition, heading, heading + offset->yaw, offset->pitch,
                                 offset->distance, &offset->offset, testOut, info->zoom, info->distance);
                    if (func_80055EE8(prevPtr, testOut) == 0) {
                        blocked = 0;
                        info->current = 1;
                        info->next = 1;
                    }
                }
                if (blocked != 0) {
                    info->next++;
                    if (indices[info->next] < 0) info->next = 2;
                    {
                        register struct Offset569EC *base __asm__("$2");
                        register s32 index __asm__("$7");
                        index = indices[info->next];
                        __asm__("lui\t%0,0x800A" : "=r"(base) : "r"(index));
                        base = (struct Offset569EC *)((s32)base - 0x52F8);
                        offset = (struct Offset569EC *)(index * 20 + (s32)base);
                    }
                    func_8005612C(&local.position, heading, heading + offset->yaw, offset->pitch,
                                 offset->distance, &offset->offset, &local.out, info->zoom, info->distance);
                    if (func_80055EE8(&local.previous, &local.out) == 0) {
                        info->current = info->next;
                    } else {
                        info->timer = 120;
                    }
                }
            }
        } else {
            info->timer = 0;
        }
    }
    {
        register struct Offset569EC *base __asm__("$2");
        register s32 index __asm__("$7");
        index = indices[info->current];
        __asm__("lui\t%0,0x800A" : "=r"(base) : "r"(index));
        base = (struct Offset569EC *)((s32)base - 0x52F8);
        offset = (struct Offset569EC *)(index * 20 + (s32)base);
    }
    yaw = func_8005605C(gCamera->yaw, heading + offset->yaw, speed);
    __asm__ __volatile__("" : "+r"(yaw));
    {
        register struct Camera569EC *camera __asm__("$2");
        camera = gCamera;
        __asm__("" : : "r"(camera) : "$5");
        pitch = func_8005605C(camera->pitch, offset->pitch, speed);
    }
    tick = elapsed;
    __asm__("" : : "r"(tick));
    scaled = tick << 9;
    __asm__("" : : "r"(scaled));
    rate = scaled >> 8;
    old = gCamera->pitch;
    if (pitch > old) {
        difference = pitch - old;
        if (difference > rate) pitch = old + rate;
    } else {
        difference = old - pitch;
        if (difference > rate) pitch = old - rate;
    }
    yaw &= 0xFFF;
    if (yaw >= 0x800) yaw -= 0x1000;
    {
        register struct Camera569EC *camera __asm__("$4");
        register s32 normalYaw __asm__("$3");
        register s32 rawYaw __asm__("$2");
        camera = gCamera;
        rawYaw = camera->yaw & 0xFFF;
        __asm__ __volatile__("" : "+r"(rawYaw));
        normalYaw = rawYaw;
        camera->yaw = normalYaw;
        if (normalYaw >= 0x800) normalYaw -= 0x1000;
        camera->yaw = normalYaw;
        difference = normalYaw - yaw;
    }
    if (ABS569EC(difference) >= 0x800) {
        if (difference > 0) yaw += 0x1000;
        else yaw -= 0x1000;
    }
    tick = elapsed;
    __asm__("" : : "r"(tick));
    scaled = tick << 10;
    __asm__("" : : "r"(scaled));
    rate = scaled >> 8;
    old = gCamera->yaw;
    if (yaw > old) {
        difference = yaw - old;
        if (difference > rate) yaw = old + rate;
    } else {
        difference = old - yaw;
        if (difference > rate) yaw = old - rate;
    }
    distance = func_800564A8(pitch);
    deltaPtr = &local.delta;
    local.delta.x = offset->offset.x - info->offset.x;
    local.delta.y = offset->offset.y - info->offset.y;
    local.delta.z = offset->offset.z - info->offset.z;
    length = func_8001A314(deltaPtr);
    tick = elapsed;
    __asm__("" : : "r"(tick));
    scaled = tick * 5 << 8;
    __asm__("" : : "r"(scaled));
    rate = scaled >> 4;
    if (length < rate) {
        info->offset = offset->offset;
    } else {
        func_8001AB78(rate, deltaPtr);
        info->offset.x += local.delta.x;
        info->offset.y += local.delta.y;
        info->offset.z += local.delta.z;
    }
    func_8005612C(&local.position, heading, yaw, pitch, distance, &info->offset,
                 &local.out, info->zoom, info->distance);
    gSetCameraRotation(yaw, pitch, 0);
    func_8001AC28(&local.out, &gCamera->axis70, info->zoom << 7, &local.out);
    {
    s32 t;
    t = info->shake;
    if (t != 0) {
        shake = t;
        if (shake >= 256) shake = 255;
        if (shake != 0) t = 1 << (31 - func_80095C14(shake));
        else t = 1;
        shake = t;
        {
            s32 random;
            random = (func_80019668() & (shake - 1)) - shake / 2;
            __asm__("" : : "r"(random) : "$4");
            func_8001AC28(&local.out, &gCamera->axis64, random << 3, &local.out);
        }
        {
            s32 random;
            random = (func_80019668() & (shake - 1)) - shake / 2;
            __asm__("" : : "r"(random) : "$4");
            func_8001AC28(&local.out, &gCamera->axis58, random << 3, &local.out);
        }
        if (info->shake > 0) {
            tick = elapsed;
    __asm__("" : : "r"(tick));
            if (info->shake > tick) { info->shake = (u16)info->shake - tick; }
            else info->shake = 0;
            __asm__("" : : "r"(tick));
        }
    }
    }
    {
        register s32 signedEffect __asm__("$2");
        register s32 lowEffect __asm__("$5");
        signedEffect = info->effect;
        lowEffect = (u16)info->effect;
        __asm__("" : "+r"(signedEffect), "+r"(lowEffect));
        if (signedEffect > 0) {
            register s32 twice __asm__("$2");
            tick = elapsed;
            __asm__("" : : "r"(tick));
            twice = tick << 1;
            lowEffect = lowEffect - twice;
            __asm__("" : "+r"(lowEffect) : "r"(tick) : "memory");
            info->effect = lowEffect;
            if (gScreenState == 4) func_8001EA20(object, (s16)lowEffect + 90);
        } else {
            func_8001EA20(object, 0);
        }
    }
    gSetCameraPosition(&local.out);
done:
    {
        register struct Camera569EC *saved __asm__("$8");
        saved = local.saved;
        gCamera = saved;
    }
}
