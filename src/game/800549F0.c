#include "game/types.h"

struct Vec549F0 {
    s32 x;
    s32 y;
    s32 z;
};

struct AnimClip549F0 {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack549F0 {
    struct AnimClip549F0 *clips;
    struct AnimClip549F0 *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

struct Sub549F0 {
    u8 pad00[0xDE];
    u8 unkDE;
    u8 unkDF;
    u8 padE0[0x66];
    s8 unk146;
    s8 unk147;
    u8 pad148[0x14];
    struct Vec549F0 velocity;
    u8 pad168[0x10];
};

struct Obj549F0 {
    u8 pad00[0x50];
    struct Vec549F0 position;
    u8 pad5C[0x1C];
    struct Vec549F0 velocity;
    s32 unk84;
    u8 pad88[0x18];
    struct Vec549F0 axis;
    struct Sub549F0 *sub;
    u16 angle;
    u8 padB2[6];
    u8 unkB8;
    u8 unkB9;
    u8 unkBA;
    u8 state;
    u8 padBC[6];
    u8 timer;
    u8 counter;
    u8 padC4[4];
    struct AnimTrack549F0 track;
};

/* Offsets are relative to the oracle's work area at sp + 0x18. */
struct Work549F0 {
    struct Vec549F0 delta;
    s32 pad0C;
    struct Vec549F0 probe;
    s32 pad1C;
    struct Vec549F0 previous;
    s32 pad2C;
    struct Vec549F0 normal;
    s32 pad3C;
    s32 magV;
    s32 magH;
    s32 surface;
    s32 pad4C;
    struct Vec549F0 from;
    s32 pad5C;
    struct Vec549F0 to;
    s32 pad6C;
};

extern s32 func_800538C8(u8 *, u32 *, s32 *, s32 *);
extern void func_80052E28(struct Obj549F0 *);
extern void func_80051AC8(struct Obj549F0 *, s32);
extern void func_8001846C(u32, s32 *, s32 *);
extern s32 func_80066F78(void *, void *, void *, void *);
extern void func_80054590(struct Obj549F0 *, s32, s32, void *, s32);
extern s32 func_80052550(struct Obj549F0 *, void *);
extern void func_8006943C(s32 *);
extern s32 func_800684AC(void *, void *, s32, s32, s32);
extern void func_80053514(struct Obj549F0 *, s32);
extern void func_80053188(struct Obj549F0 *, s32);
extern struct AnimClip549F0 *func_800448D8(struct AnimTrack549F0 *, s32, s32, u8);
extern void func_8001A6F8(void *, void *);
extern void func_8001A180(void *);
extern s32 func_80019FCC(s32 *);
extern s32 func_80095900(s32);
extern s32 func_8001A7AC(s32);
extern s32 func_80044B34(struct Obj549F0 *, struct AnimTrack549F0 *, s32, s32);
extern void func_80047088(s32, struct Obj549F0 *);
extern s32 func_8001A314(s32 *);
extern void func_800175F8(void *, s32);
extern s32 func_8005220C(struct Obj549F0 *);
extern void func_80052708(struct Obj549F0 *, s32);
extern s32 func_80052F54(struct Obj549F0 *, s32);

s32 func_800549F0(struct Obj549F0 *selfArg, s32 unused, s32 speedArg, u32 argFlags)
{
    register struct Obj549F0 *self __asm__("$17") = selfArg;
    s32 speed = speedArg;
    struct Work549F0 w;
    s32 wflags;
    s32 fallbackFlags;
    s32 fallbackSurface;
    register struct Sub549F0 *sub __asm__("$20");
    register s32 move __asm__("$21");
    register s32 anim __asm__("$19");
    s32 action;
    register s32 spare __asm__("$23");
    register s32 input __asm__("$30");
    s32 impactTotal;
    s32 remaining;
    u8 status;
    struct Vec549F0 *normalPtr;
    s32 lane;
    s32 st;

    (void)unused;
    impactTotal = 0;
    sub = self->sub;
    move = 0;
    status = (u8)func_800538C8((u8 *)sub, &argFlags, &w.magV, &w.magH);

    if ((argFlags & 0x0A000000) == 0x08000000 &&
        (st = self->state) != 3 &&
        (u32)(st - 0x0D) >= 2 &&
        (self->unkB9 & 0x40) == 0) {
        w.probe.x = self->position.x + (self->axis.x >> 3);
        w.probe.y = self->position.y + 0x10000;
        w.probe.z = self->position.z + (self->axis.z >> 3);
        lane = func_80066F78(&w.probe, (void *)move, (void *)move,
                            &w.surface) - self->position.y;
        move = 0x0F;
        if (lane >= 0x0AE2) {
            move = 0x0A;
            if ((sub->unkDF & 1) == 0) {
                if (lane >= 0x2334) {
                    self->unkB9 |= 8;
                }
            }
        }
    } else if (argFlags & 0x02000000) {
        if (argFlags & 0x40000000) {
            move = 1;
        } else if ((s32)argFlags < 0) {
            move = 2;
        } else if (argFlags & 0x08000000) {
            move = 9;
        } else if (argFlags & 0x00200000) {
            move = 6;
        } else if (argFlags & 0x00100000) {
            move = 5;
        }
        if (argFlags & 0x20000000) {
            move |= 0x40;
        } else if (argFlags & 0x10000000) {
            move |= 0x20;
        }
    } else {
        if (argFlags & 0x40000000) {
            move = 3;
        } else if ((s32)argFlags < 0) {
            move = 4;
        } else if (argFlags & 0x00200000) {
            move = 8;
        } else if (argFlags & 0x00100000) {
            move = 7;
        }
        if (argFlags & 0x20000000) {
            move |= 0x40;
        } else if (argFlags & 0x10000000) {
            move |= 0x20;
        }
    }

    remaining = speed;
    if (speed <= 0) {
        goto end;
    }
    input = move & 0x1F;
    normalPtr = &w.normal;

loop:
    anim = 0;
    if ((self->unkB9 & 1) == 0) {
        if ((u32)(self->state - 1) < 8 || self->state == 0x0E) {
            if (move == 0) {
                self->state = 0;
            }
        }
    }

    spare = 0;
    if (self->unkB9 & 0x41) {
        self->unkB9 &= 0xDF;
    }
    if (self->state == 0 && input == 0) {
        self->unkB9 &= 0xCD;
    }

    action = 1;
    if ((self->unkB9 & 0x10) == 0) {
        action = (self->unkB9 >> 4) & 2;
    }
    if ((argFlags & 0x08000000) && self->state == 3 && action == 0) {
        action = 2;
        goto after_action;
    }

    if (*(u8 *)((u8 *)self->track.clip + 3) & 2) {
        st = self->unkB9;
        if (st & 2) {
            action = 1;
            self->unkB9 = st & 0xFD;
            if (input != 0) {
                self->state = input;
            }
        }
    } else {
        if (input != self->state) {
            if (self->track.index != 0 || (self->unkB9 & 2) != 0) {
                action = 1;
                self->unkB9 &= 0xFD;
                goto after_action;
            }
        }
        self->state = input;
    }

after_action:
    {
        s32 bits;

        st = self->unkB9;
        bits = st & 0xFD;
        st &= 1;
        self->unkB9 = bits;
    }
    if (st == 0) {
        register s32 turn __asm__("$4");

        turn = 0x20;
        if ((move & 0x40) == 0) {
            turn = 0;
            if (move & 0x20) {
                turn = -0x20;
            }
        }
        if (status != 0) {
            turn = (turn * w.magH) >> 7;
        }
        if (turn != 0) {
            turn -= (turn * self->unk84) >> 9;
            self->angle = self->angle + turn;
            anim = 0x0E;
            if (turn > 0) {
                anim = 0x0F;
            }
        }
    }

    if (self->unkB8 != 0) {
        u8 st;

        st = self->state;
        if (st != input) {
            if ((u32)(self->state - 1) < 8 || self->state == 0x0E) {
                self->state = 0;
            }
        }
        func_80052E28(self);
        self->unkB8 = 0;
    }
    {
        u32 angle;

        angle = self->angle & 0x0FFF;
        self->angle = angle;
        func_8001846C(angle, &self->axis.x, &self->axis.z);
    }
    self->axis.y = 0;
    w.delta.y = 0;
    if (self->timer != 0) {
        self->timer--;
    }

    speed = 0;
    lane = speed;
    if (self->unkB9 & 0x80) {
        if (self->state != 9) {
            func_80051AC8(self, speed);
            self->unkB9 &= 0x7F;
        }
    }

    {
        s32 flags;

        flags = self->unkB9;
        if (flags & 1) {
            goto timer_handler;
        }
        switch (self->state) {
        case 9:
            spare = 1;
            anim = 3;
            if ((self->unkB9 & 0x80) == 0) {
                func_80051AC8(self, spare);
            }
            self->unkB9 |= 0x80;
            break;
        case 10:
            anim = 4;
            break;
        case 11:
            {
                s32 vertical;

                vertical = 0x78000;
                if (sub->unkDF & 1) {
                    vertical = 0xA0000;
                }
                anim = 4;
                w.delta.y = vertical;
                self->timer = 0x0A;
                self->state = 0x0C;
            }
            break;
        case 16:
            {
                s32 vertical;

                vertical = 0x58000;
                if (sub->unkDF & 1) {
                    vertical = 0x75555;
                }
                speed = 0x40000;
                anim = 0x11;
                w.delta.y = vertical;
                self->state = 0x11;
            }
            break;
        case 15:
        case 17:
            anim = 0x11;
            break;
        case 13:
            {
                s32 vertical;

                speed = 0x18000;
                vertical = 0x64000;
                if (sub->unkDF & 1) {
                    vertical = 0x85555;
                }
                anim = 8;
                w.delta.y = vertical;
                self->state = 0x0E;
            }
            break;
        case 18:
            spare = 1;
            anim = 5;
            break;
        case 3:
            speed = 0x18000;
            /* Retail case 14 enters after this assignment. */
        case 14:
            anim = 8;
            break;
        case 4:
            speed = -0x18000;
            anim = 9;
            break;
        case 1:
            speed = 0xA000;
            anim = 6;
            break;
        case 2:
            speed = -0xA000;
            anim = 7;
            break;
        case 6:
            anim = 2;
            speed = 0xA000;
            lane = 1;
            break;
        case 5:
            anim = 1;
            speed = -0xA000;
            lane = 1;
            break;
        case 8:
            anim = 0x0B;
            speed = 0x10000;
            lane = 1;
            break;
        case 7:
            anim = 0x0A;
            speed = -0x10000;
            lane = 1;
            break;
        case 0:
            spare = 1;
            if (move == 0) {
                self->counter++;
                if (self->counter == 0) {
                    anim = 0x10;
                    break;
                }
                if (self->track.index == 0) {
                    break;
                }
                if (self->unkBA != 0x10) {
                    break;
                }
                anim = self->unkBA;
            }
            self->counter = 0;
            break;
        case 12:
        default:
            break;
        }
        goto dispatch_done;

timer_handler:
        anim = self->unkBA;
        if (self->timer == 1 && self->state == 0x0C) {
            if ((flags & 8) == 0) {
                speed = 0x1CCCC;
            }
            self->unkB9 = flags & 0xF7;
        }
    }

dispatch_done:
    {
        s32 direction;

        direction = 0;
        if ((u32)(self->state - 1) < 4) {
            if ((argFlags & 0x08000000) == 0) {
                if (status != 0) {
                    speed = (speed * w.magV) >> 7;
                }
                if (argFlags & 0x00200000) {
                    direction = 1;
                } else if (argFlags & 0x00100000) {
                    direction = 2;
                }
            }
        }
        func_80054590(self, speed, lane, &w.delta, direction);
    }

    {
        s32 collision;

        collision = func_80052550(self, &w.delta);
        sub->velocity.x += w.delta.x;
        sub->velocity.y += w.delta.y;
        sub->velocity.z += w.delta.z;
        if (sub->velocity.y < -0xC0000) {
            sub->velocity.y = -0xC0000;
        }
        if (collision == 0 && spare != 0) {
            s32 value;
            s32 sign;
            s32 component;
            s32 difference;
            s32 response;

            s32 vx, vy, vz;

            vx = sub->velocity.x;
            vy = sub->velocity.y;
            speed = ((vx >> 31) + vx ^ (vx >> 31));
            component = ((vy >> 31) + vy ^ (vy >> 31));
            difference = component - speed;
            speed = component - ((difference >> 31) & difference);
            vz = sub->velocity.z;
            component = ((vz >> 31) + vz ^ (vz >> 31));
            difference = component - speed;
            speed = component - ((difference >> 31) & difference);
            response = self->sub->unk147;
            if ((speed <= 0x7FFF && response == 0) ||
                speed < (response << 9)) {
                sub->velocity.x = sub->velocity.y = sub->velocity.z = 0;
            }
        }
    }

    w.previous = self->position;
    self->position.x += sub->velocity.x >> 8;
    self->position.y += sub->velocity.y >> 8;
    self->position.z += sub->velocity.z >> 8;
    func_8006943C(&self->position.x);

    if (sub->velocity.y >= 0x11) {
        w.from.x = w.previous.x;
        w.from.y = w.previous.y + 0x24C0;
        w.from.z = w.previous.z;
        w.to.x = self->position.x;
        w.to.y = self->position.y + 0x24C0;
        w.to.z = self->position.z;
        if (func_800684AC(&w.from, &w.to, 0, 0, 1) & 7) {
            s32 vx;
            s32 vz;

            vx = sub->velocity.x;
            vz = sub->velocity.z;
            sub->velocity.y = 0;
            sub->velocity.x = vx / 2;
            sub->velocity.z = vz / 2;
        }
    }

    w.probe.x = self->position.x;
    w.probe.y = self->position.y + 0x0CCC;
    w.probe.z = self->position.z;
    lane = func_80066F78(&w.probe, normalPtr, &wflags, &w.surface);
    if (self->sub->unkDE != 0) {
        lane = self->position.y - 0xC0;
        self->sub->velocity.y = 0;
    }
    func_80053514(self, w.surface);
    self->sub->unk147 = 0;
    self->sub->unk146 = 0;
    spare = (s32)0xFFFFFF3F;
    wflags &= spare;
    if (wflags != 0) {
        func_80053188(self, wflags);
    }

    speed = (s32)((u8 *)sub + 0x168);
    func_8001A6F8(normalPtr, (void *)speed);
    func_8001A180((void *)speed);

    if (self->position.y < lane) {
        if (self->unkB9 & 1) {
            s32 vertical;
            s32 limit;

            self->unkB9 = (self->unkB9 & 0xFE) | 2;
            vertical = sub->velocity.y;
            sub->velocity.y = 0;
            limit = 0x50000;
            speed = -vertical;
            if (*(u8 *)((u8 *)self->track.clip + 3) & 0x10) {
                limit = 0x80000;
            }
            if (limit < speed) {
                s32 damage;

                self->state = 0x12;
                damage = func_80052F54(self, speed);
                impactTotal += damage;
            }
        } else if (self->state == 0x0C && sub->velocity.y < -0x40) {
            sub->velocity.y = 0;
        }
    } else if (self->position.y - lane >= 0x333) {
        goto above_ground;
    }
    self->position.y = lane;
    goto after_ground;

above_ground:
    if ((u32)(self->state - 1) < 2 || self->state == 5) {
        if ((self->unkB9 & 0x40) == 0 && self->sub->unkDE == 0) {
            s32 floor;

            anim = 0;
            self->state = 0;
            w.probe.x = w.previous.x;
            w.probe.y = w.previous.y + 0x0CCC;
            w.probe.z = w.previous.z;
            floor = func_80066F78(&w.probe, (void *)anim,
                                 &fallbackFlags, &fallbackSurface);
            if (self->position.y < floor ||
                self->position.y - floor < 0x333) {
            {
                s32 fb;
                s32 masked;

                fb = fallbackFlags;
                self->position = w.previous;
                masked = fb & spare;
                fallbackFlags = masked;
                if (masked != 0) {
                    func_80053188(self, masked);
                }
                sub->velocity.x = 0;
                sub->velocity.z = 0;
                goto after_ground;
            }
            }
        }
    }
    self->unkB9 |= 1;

after_ground:
    if (self->state == 0x0E) {
        s32 bits;

        bits = self->unkB9;
        if ((bits & 2) == 0) {
            self->unkB9 = bits | 1;
        }
    }
    func_80052708(self, func_8005220C(self));

    if (anim != self->unkBA) {
        goto change_animation;
    }
    if (self->track.index != 0) {
        goto animation_done;
    }
    if (anim == 0) {
        goto animation_done;
    }
change_animation:
    func_800448D8(&self->track, anim, 1, 1);
    self->unkBA = anim;

animation_done:
    if (self->state == 0x0A) goto fixed_speed;
    if (self->state == 0x0F) goto fixed_speed;
    if (self->state == 0x0B) goto fixed_speed;
    if (self->state == 0x0C) goto fixed_speed;
    if (self->state == 0x12) goto fixed_speed;
    if (self->state == 9) goto fixed_speed;
    if (self->state == 0x11) goto fixed_speed;
    goto measured_speed;

fixed_speed:
    speed = 0xA0;
    goto speed_done;

measured_speed:
    speed = func_80095900(func_80019FCC(&sub->velocity.x)) << 2;
    speed >>= 8;
    if (self->sub->unk147 != 0) {
        speed *= 4 - self->sub->unk147;
    }
    st = self->state;
    if (st == 0x0E) {
        if (speed < 0x300) {
            speed = 0x300;
        }
    } else if ((u32)(st - 1) < 4) {
        if (speed < 0x100) {
            speed = 0x100;
        }
    } else if (speed < 0x1E) {
        speed = 0x1E;
    }

speed_done:
    w.from.x = w.normal.x;
    w.from.y = w.normal.z;
    {
        s32 magnitude;

        magnitude = func_8001A7AC((s32)&w.from);
        self->unkB9 &= 0xBF;
        if (0xA000 < magnitude) {
            self->unkB9 |= 0x40;
        }
    }

    if (action == 2) {
        if ((self->unkB9 & 1) == 0) {
            if ((self->unkB9 & 0x40) == 0) {
                s32 frame;
                s32 index;

                frame = self->track.frame;
                index = 0x37;
                if ((u32)(frame - 2) >= 4 &&
                    (u32)(frame - 0x12) >= 0x0E) {
                    index = 0x36;
                }
                func_800448D8(&self->track, index, 1, 1);
                frame = self->unkB9;
                self->state = 0x0D;
                frame &= 0xCF;
                self->unkB9 = frame;
            }
            action = 0;
        }
    }

    lane = func_80044B34(self, &self->track, speed, action);
    if (lane & 1) {
        self->unkB9 &= 0xEF;
    } else if (action != 0) {
        self->unkB9 |= 8 << action;
    }

    speed = (s32)((u32)lane >> 1);
    while (speed != 0) {
        s32 event;

        event = speed & 0x3F;
        speed >>= 6;
        func_80047088(event, self);
    }

    if (*(u8 *)((u8 *)self->track.clip + 3) & 4) {
        if (self->state == 0x0A) {
            self->state = 0x0B;
        } else if (self->state == 0x0F) {
            self->state = 0x10;
        }
    }
    if (self->track.index == 0 && (lane & 1)) {
        self->state = 0;
        func_80047088(0x27, self);
    }

    if (--remaining > 0) {
        goto loop;
    }

end:
    self->velocity = sub->velocity;
    self->unk84 = func_8001A314(&self->velocity.x) >> 11;
    if (self->unk84 == 0) {
        self->velocity = self->axis;
    }
    func_800175F8((u8 *)self + 0x28, self->track.frame);
    return impactTotal;
}
