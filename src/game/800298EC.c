#include "game/types.h"

extern u32 D_8009FA00;
extern s32 func_800452C8(void);

typedef struct {
    s16 value;
    u8 state;
    u8 reserved[5];
} QueueSlot298EC;

typedef struct {
    u16 length;
    s16 baseIndex;
    QueueSlot298EC slots[8];
} Queue298EC;

extern Queue298EC D_800A0BC0;

void func_800298EC(s16 arg0)
{
    if (D_8009FA00 < 5) {
        if (func_800452C8() != 0) {
            s16 pending = D_800A0BC0.length;
            if (pending < 8) {
                s32 slot = (D_800A0BC0.baseIndex + pending) & 7;
                D_800A0BC0.slots[slot].value = arg0;
                D_800A0BC0.slots[slot].state = 0;
                D_800A0BC0.length++;
            }
        }
    }
}
