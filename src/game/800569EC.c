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

extern struct Mode569EC *D_8009FA94;
extern struct Override569EC *D_8009F9A0;
extern s32 D_8009FA84;
extern struct Camera569EC *D_8009F9E4;
extern struct State569EC D_8009AEC4[];
extern struct Offset569EC D_8009AD08[];
extern s32 D_8009FA00;
extern void func_8005780C(void *, struct Vec569EC *);
extern s32 func_80031B40(struct Vec569EC *, void *);
extern s32 func_80056530(struct Obj569EC *, s32, s32);
extern s32 func_80056870(struct Obj569EC *);
extern u8 func_80056994(struct Info569EC *, s32);
extern void func_80017B60(s32);
extern void func_8005678C(struct Obj569EC *, s32, struct Vec569EC *, s32);
extern void func_8005612C(struct Vec569EC *, s32, s32, s32, s32, struct Vec569EC *, struct Vec569EC *, s32, s32);
extern s32 func_80055EE8(struct Vec569EC *, struct Vec569EC *);
extern s32 func_8005605C(s32, s32, s32);
extern s32 func_800564A8(s32);
extern s32 func_8001A314(struct Vec569EC *);
extern void func_8001AB78(s32, struct Vec569EC *);
extern void func_80017860(s32, s32, s32);
extern void func_8001AC28(struct Vec569EC *, struct Vec569EC *, s32, struct Vec569EC *);
extern s32 func_80095C14(s32);
extern s32 func_80019668(void);
extern void func_8001EA20(struct Obj569EC *, s32);
extern void func_800177AC(struct Vec569EC *);

#define ABS569EC(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

#define RELOAD_OBJECT() (*(struct Obj569EC *volatile *)&object)

void func_800569EC(struct Obj569EC *object, volatile s32 elapsed, volatile s32 speed)
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
    } else if (info->mode != 0 && D_8009FA94->mode != 1) {
        if (info->selected == 0) {
            info->selected = func_80056870(object);
        }
    } else {
        if (info->selected != 0) info->selected = 0;
    }
    {
        register struct Override569EC *override __asm__("$7");
        register s32 overrideIndex __asm__("$2");
        override = D_8009F9A0;
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
    if (D_8009FA84 > 0) state = func_80056994(info, elapsed);
    {
        register struct Camera569EC *saved __asm__("$8");
        register s32 id __asm__("$4");
        saved = D_8009F9E4;
        id = info->camera;
        local.saved = saved;
        func_80017B60(id);
    }
    if (selected > 0) {
        func_8005678C(object, selected, &local.out, 1);
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
    yaw = func_8005605C(D_8009F9E4->yaw, heading + offset->yaw, speed);
    __asm__ __volatile__("" : "+r"(yaw));
    {
        register struct Camera569EC *camera __asm__("$2");
        camera = D_8009F9E4;
        __asm__("" : : "r"(camera) : "$5");
        pitch = func_8005605C(camera->pitch, offset->pitch, speed);
    }
    tick = elapsed;
    __asm__("" : : "r"(tick));
    scaled = tick << 9;
    __asm__("" : : "r"(scaled));
    rate = scaled >> 8;
    old = D_8009F9E4->pitch;
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
        camera = D_8009F9E4;
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
    old = D_8009F9E4->yaw;
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
    func_80017860(yaw, pitch, 0);
    func_8001AC28(&local.out, &D_8009F9E4->axis70, info->zoom << 7, &local.out);
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
            func_8001AC28(&local.out, &D_8009F9E4->axis64, random << 3, &local.out);
        }
        {
            s32 random;
            random = (func_80019668() & (shake - 1)) - shake / 2;
            __asm__("" : : "r"(random) : "$4");
            func_8001AC28(&local.out, &D_8009F9E4->axis58, random << 3, &local.out);
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
            if (D_8009FA00 == 4) func_8001EA20(object, (s16)lowEffect + 90);
        } else {
            func_8001EA20(object, 0);
        }
    }
    func_800177AC(&local.out);
done:
    {
        register struct Camera569EC *saved __asm__("$8");
        saved = local.saved;
        D_8009F9E4 = saved;
    }
}
