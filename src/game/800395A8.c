#include "game/types.h"

struct Unk395A8Child {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
};

struct Unk395A8Self {
    u8 pad00[0x1C];
    u16 unk1C;
    u8 pad1E[6];
    u32 unk24;
    u8 pad28[0x130];
    struct Unk395A8Child *unk158;
};

extern void func_80039504(s32 arg0, void *arg1, void *arg2, s32 arg3);
extern s32 func_8002AD58(s32 message, void *object, void *data, s32 arg3);
extern void func_800382B4(s32 arg0, void *arg1, void *arg2, s32 arg3);
extern void func_80038A00(void *arg0, void *arg1);
extern void func_8003BA50(struct Unk395A8Child *object, s32 value);

s32 func_800395A8(s32 message, struct Unk395A8Self *object, void *arg2, s32 arg3)
{
    register s32 msg __asm__("$17") = message;
    register struct Unk395A8Self *self __asm__("$16") = object;
    register void *data __asm__("$18") = arg2;
    register s32 extra __asm__("$19") = arg3;
    s32 previous;

    switch (msg) {
    case 1:
        return 0x15C;
    case 2:
        func_80039504(msg, self, data, extra);
        return 0;
    case 3:
        if (self->unk24 & 0x20) {
            return 0;
        }
        func_8002AD58(msg, self, data, extra);
        if (self->unk158 != 0) {
            func_8002AD58(msg, self->unk158, data, extra);
        }
        return 0;
    case 6:
        func_800382B4(msg, self, data, extra);
        if (self->unk158 != 0) {
            self->unk158->unk1C = self->unk1C;
            func_80038A00(self->unk158, data);
        }
        return 0;
    case 9:
    case 11:
    case 14:
        func_800382B4(msg, self, data, extra);
        if (self->unk158 != 0) {
            if (self->unk24 & 0x02000000) {
                self->unk158->unk24 |= 0x02000000;
            } else {
                self->unk158->unk24 &= 0xFDFFFFFF;
            }
            if (self->unk158->unk20 != 0 && (self->unk158->unk24 & 0x40000) == 0) {
                previous = self->unk158->unk1C;
                self->unk158->unk1C = self->unk1C;
                func_8003BA50(self->unk158, previous);
            } else {
                self->unk158->unk1C = self->unk1C;
            }
        }
        return 0;
    case 5:
        if (self->unk158 != 0) {
            self->unk158->unk1C = -1;
        }
    case 4:
    case 7:
    case 8:
    case 10:
    case 12:
    case 13:
    default:
        func_800382B4(msg, self, data, extra);
        break;
    }
}
