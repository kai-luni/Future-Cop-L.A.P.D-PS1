#include "game/types.h"

struct InputAction267C8 {
    u32 buttons;
    s32 phase;
};

struct State267C8 {
    u8 pad0[0x30];
    u32 buttons;
    u8 pad34[0x304 - 0x34];
    s32 unk304;
    s32 unk308;
    s32 unk30C;
    u8 pad310[0x312 - 0x310];
    s8 selected;
    u8 pad313;
    s16 unk314;
    s16 unk316;
};

struct Obj267C8 {
    u8 pad0[0x50];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    u8 pad5C[0xA0 - 0x5C];
    u8 unkA0[0xC];
    struct State267C8 *state;
};

struct SoundState267C8 {
    u8 pad0[0xC];
    s32 unk0C;
    u8 pad10[0x5C - 0x10];
    s32 unk5C;
};

union SpeedValue267C8 {
    u16 half;
    s32 word;
};

extern struct InputAction267C8 D_800A0AF0[];
extern struct InputAction267C8 D_800A0B00[];
extern struct InputAction267C8 D_800A0B10[];
extern struct InputAction267C8 D_800A0B20[];
extern struct InputAction267C8 D_800A0AA0[];

extern s32 D_8009F670[];
extern s32 D_8009FA58[];
extern s32 D_8009FA6C;
extern s32 D_8009FA0C;
extern u32 D_8009FA2C;
extern union SpeedValue267C8 D_8009F9FC;
extern struct SoundState267C8 *D_8009FA94;
extern u8 *D_8009F2D8;
extern u8 *D_8009F2DC;
extern u8 *D_8009F2E0;

extern void func_8005780C(u32 buttons, struct InputAction267C8 *action);
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_80025F58(void);
extern s32 func_80049340(u8 *text);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_8001C214(u32 color, s32 x, s32 y, s32 w, s32 h, s32 blend);
extern void func_800260B8(struct Obj267C8 *obj, s32 slot, s32 arg2, s32 selection, s32 active);
extern void func_800576A8(u32 *buttons, s32 slot);
extern void func_8005AB8C(struct Obj267C8 *obj, void *arg1, void *arg2);
extern void func_8006680C(struct Obj267C8 *obj, void *arg1);

void func_800267C8(struct Obj267C8 *obj, s32 slot, s32 active, s32 selection)
{
    register struct State267C8 *state __asm__("$19");
    register u32 off __asm__("$16");
    register struct InputAction267C8 *first __asm__("$20");
    register struct InputAction267C8 *second __asm__("$18");
    register struct InputAction267C8 *third __asm__("$17");
    register u8 *text __asm__("$20");
    register s32 textWidth __asm__("$18");
    register s32 center __asm__("$17");
    s32 choice;
    s32 action;


    off = (u32)slot << 3;
    state = obj->state;
    {
        register u32 base __asm__("$2");
        base = (u32)D_800A0AF0;
        first = (struct InputAction267C8 *)(off + base);
    }
    func_8005780C(state->buttons, first);
    {
        register u32 base __asm__("$2");
        base = (u32)D_800A0B00;
        second = (struct InputAction267C8 *)(off + base);
    }
    func_8005780C(state->buttons, second);
    {
        register u32 base __asm__("$2");
        base = (u32)D_800A0B10;
        third = (struct InputAction267C8 *)(off + base);
    }
    func_8005780C(state->buttons, third);
    func_8005780C(state->buttons,
                  (struct InputAction267C8 *)(off + (u32)D_800A0B20));
    off += (u32)D_800A0AA0;
    func_8005780C(state->buttons, (struct InputAction267C8 *)off);

    action = 0;
    choice = action;
    if (((struct InputAction267C8 *)off)->phase == 0) {
        action = 1;
        choice = action;
    }
    if (first->phase == 0) {
        choice = 2;
    }
    if (second->phase == 0) {
        choice = 3;
    }
    if (third->phase == 0) {
        choice = 4;
    }

    if (obj->state->selected == 1 && choice != 0) {
        {
            s32 v;
            register s32 offset __asm__("$3");
            offset = slot << 2;
            __asm__ volatile("" : : "r"(offset));
            v = *(s32 *)((u32)D_8009F670 + offset) * 4;
            v = v - 1;
            *(s32 *)((u32)D_8009F670 + offset) = v + choice;
        }
    }

    if (action != 0 && obj->state->selected == -1) {
        if (D_8009FA94->unk5C == 0 ||
            (D_8009FA94->unk5C == 1 &&
             (D_8009FA94->unk0C == 1 ||
              (D_8009FA94->unk0C == 2 && D_8009FA6C == 1)))) {
            func_80046C84(0x3E, D_8009FA58[slot]);
        }
        D_8009FA6C++;
        obj->state->selected = 0;
        func_80025F58();
        D_8009F670[slot] = 0;
    }

    off = 0x280;
    if (D_8009FA0C == 2) {
        off = 0x140;
    }

    if (active != 0 && (D_8009FA2C & 0x100) != 0) {
        text = D_8009F2D8;
        {
            s32 x;
            x = 0x14;
            if (slot != 1) {
                x = ((s32)(off - func_80049340(text))) / 2 - 0x14;
            }
            func_80049100(text, x, 0x10, 9);
        }
    }

    if (obj->state->selected >= 0) {
        func_800260B8(obj, slot, selection, off,
                     D_800A0B00[slot].phase == 0);
        text = D_8009F2DC;
    } else {
        text = D_8009F2E0;
    }

    {
        register u8 *arg __asm__("$4");
        register s32 color __asm__("$4");
        register s32 result __asm__("$2");
        result = func_80049340(text);
        arg = text;
        __asm__ volatile("" : "+r"(result) : "r"(arg));
        textWidth = result;
        result = func_80049340(arg);
        color = 1;
        __asm__ volatile("" : "+r"(result), "+r"(color));
        off -= result;
        __asm__ volatile("" : "+r"(off));
        off += off >> 31;
        __asm__ volatile("" : "+r"(off));
        center = (s32)off >> 1;
        {
            s32 blend;
            __asm__("addu %0,%1,$zero" : "=r"(blend) : "r"(color));
            func_8001C214(color, center - 6, 0x1AE, textWidth + 0xC, 0x1A, blend);
        }
    }
    {
        register u8 *arg __asm__("$4");
        arg = text;
        __asm__ volatile("" : "+r"(off) : "r"(arg));
    }
    center += off >> 31;
    __asm__ volatile("" : "+r"(center));
    func_80049100(text, center >> 1, 0xD8, 9);

    if (state->buttons & 0x02000000) {
        struct VectorCopy267C8 { s32 x, y, z; };
        *(struct VectorCopy267C8 *)&state->unk304 = *(struct VectorCopy267C8 *)&obj->unk50;
    }

    func_800576A8(&state->buttons, slot);

    if (obj->state->selected < 0) {
        u32 buttons;

        buttons = state->buttons;

        if (buttons & 0x20000000) {
            if ((s16)state->unk314 < 0) {
                state->unk314 = 0;
            }
            state->unk314 += D_8009F9FC.half;
            if ((s16)state->unk314 > 0x200) {
                state->unk314 = 0x200;
            }
        } else if (buttons & 0x10000000) {
            if ((s16)state->unk314 > 0) {
                state->unk314 = 0;
            }
            state->unk314 -= D_8009F9FC.half;
            if ((s16)state->unk314 < -0x200) {
                state->unk314 = -0x200;
            }
        } else {
            state->unk314 = 0;
        }

        if (buttons & 0x80000000) {
            if ((s16)state->unk316 < 0) {
                state->unk316 = 0;
            }
            state->unk316 += D_8009F9FC.half;
            if ((s16)state->unk316 > 0x200) {
                state->unk316 = 0x200;
            }
        } else if (buttons & 0x40000000) {
            if ((s16)state->unk316 > 0) {
                state->unk316 = 0;
            }
            state->unk316 -= D_8009F9FC.half;
            if ((s16)state->unk316 < -0x200) {
                state->unk316 = -0x200;
            }
        } else {
            state->unk316 = 0;
        }

        {
            s32 dz;
            s32 dx;

            dz = state->unk314 * D_8009F9FC.word;
            dx = state->unk316 * D_8009F9FC.word;
            state->unk30C += dz << 2;
            state->unk304 += dx << 2;
        }
    }

    func_8005AB8C(obj, &obj->unk50, &obj->unkA0);
    func_8006680C(obj, &state->unk304);
}
