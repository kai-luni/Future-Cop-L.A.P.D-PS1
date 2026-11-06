#include "game/types.h"

struct Unk6BD48Obj {
    u8 unk00[0xAC];
    u8 *ctl;
};

struct Unk6BD48Tgt {
    u8 unk00[0x8];
    s16 unk8;
    u8 unk0A[0x1E];
    s16 unk28;
};

struct Unk6BD48State {
    u8 unk00[0x8];
    s32 unk8;
    u8 unk0C[0x8C];
    s32 unk98;
};

extern struct Unk6BD48State *D_8009FA94;

s32 func_8006BD48(struct Unk6BD48Obj *obj, struct Unk6BD48Tgt *tgt, s32 flag)
{
    s32 val;
    s32 state;

    if ((flag & 0xFF) != 0) {
        val = tgt->unk28;
    } else {
        val = tgt->unk8;
    }

    if (*(s8 *)(obj->ctl + 0xC1) == 0) {
        state = D_8009FA94->unk8;
    } else {
        state = D_8009FA94->unk98;
    }

    switch (state) {
    case 0:
        break;
    case 1:
        val = val + (val >> 2);
        break;
    case 2:
        val = val + (val >> 1);
        break;
    case 3:
        val = 0x270F;
        break;
    }

    return val;
}
