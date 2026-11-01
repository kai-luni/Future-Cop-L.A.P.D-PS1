#include "game/types.h"

typedef struct Unk6C34CControl Unk6C34CControl;

typedef struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0xA0];
    Unk6C34CControl *control;
} Entity;

struct Unk6C34CControl {
    u8 unk00[0x68];
    s32 unk68;
    Entity *unk6C;
    Entity *unk70;
};

extern Entity *func_8006C224(Entity *source, Entity *exclude, s32 maxDist, s32 flags);

void func_8006C34C(Entity *object, s32 arg1)
{
    Unk6C34CControl *control;
    Entity *result;

    control = object->control;
    if (((u8)arg1) == 0 || control->unk6C == 0) {
        control->unk70 = control->unk6C;
    }
    result = func_8006C224(object, control->unk70, 0x18000, 0xF9EE);
    if (result != 0) {
        control->unk6C = result;
        control->unk68 = result->unk08;
        if (((u8)arg1) != 0) {
            control->unk70 = 0;
        }
    }
}
