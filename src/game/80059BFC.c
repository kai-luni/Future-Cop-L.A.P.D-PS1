#include "game/types.h"

typedef struct {
    u8 unk00;
    s8 unk01;
    s8 unk02;
    s8 unk03;
    s8 unk04;
    s8 unk05;
    s8 unk06;
    s8 unk07;
    s16 sizes[2];
    u8 *ptrs1[2];
    u8 *ptrs2[2];
    u8 *end1;
    u8 *end2;
    u8 data[1];
} LoadedData;

typedef struct {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x1C];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[0x58];
    s32 unk98;
} SomeState;

extern s32 D_8009FA84;
extern SomeState *D_8009FA94;
extern LoadedData *D_8009F7D0;
extern u8 D_80011B00[];

extern void func_800197A8(s32 seed);
extern s32 func_8008CCC0(u8 *str);
extern void func_8008CD30(u8 *dst, u8 *fmt, u8 *name, s32 arg3, s32 arg4);
extern void *func_80059BA0(s32 name);

void func_80059BFC(s32 countArg, s32 seed, u8 *nameArg)
{
    u8 buffer[0x100];
    register s32 count __asm__("$19") = countArg;
    register u8 *name __asm__("$16") = nameArg;
    register s32 len __asm__("$17");
    register s32 wasExt __asm__("$18");
    register LoadedData *data __asm__("$6");
    register SomeState *state __asm__("$3");
    register u8 *ptr __asm__("$5");
    register s32 i __asm__("$4");

    func_800197A8(seed);
    if (D_8009FA84 <= 0) {
        return;
    }

    len = func_8008CCC0(name);
    wasExt = 0;
    if (name[len - 4] == '.' && name[len - 3] == 'm' &&
        name[len - 2] == 'i' && name[len - 1] == 's') {
        wasExt = 1;
        name[len - 4] = 0;
    }

    func_8008CD30(buffer, D_80011B00, name, count + 0x40, D_8009FA84);
    if (wasExt) {
        register u8 *restore __asm__("$3") = (u8 *)(len + (s32)name);
        restore[-4] = '.';
    }

    data = func_80059BA0((s32)buffer);
    D_8009F7D0 = data;
    if (data != 0) {
        ptr = data->data;
        state = D_8009FA94;
        state->unk08 = data->unk01;
        state->unk98 = data->unk01;
        state->unk28 = data->unk02;
        state->unk2C = data->unk03;
        state->unk30 = data->unk04;
        state->unk34 = data->unk05;
        state->unk38 = data->unk06;
        state->unk3C = data->unk07;

        i = 0;
        if (count > 0) {
            register u8 **slot __asm__("$3") = (u8 **)data;
            do {
                slot[3] = ptr;
                slot[5] = ptr;
                ptr += data->sizes[0] * 4;
                data = (LoadedData *)((u8 *)data + 2);
                i++;
                slot++;
            } while (i < count);
        }
        D_8009F7D0->end1 = ptr;
        D_8009F7D0->end2 = ptr;
    } else {
        __asm__("" : "+r"(data));
        D_8009FA84 = 0;
    }
}
