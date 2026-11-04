#include "game/types.h"

struct Slot {
    u8 pad00[4];
    u8 value;
    u8 pad05[3];
};

struct Entry {
    u8 pad00[0x18];
    u16 flags;
    u8 column;
};

extern struct Entry *D_800A0C70[];
extern s32 func_80019668(void);

u8 func_8003B4E0(u8 index, u8 slot, u8 mode)
{
    if (!(D_800A0C70[index]->flags & 1)) {
        if (slot == 0 && mode != 0) {
            if (func_80019668() & 1) {
                return D_800A0C70[index]->column;
            }
        }
    }
    return ((u8 *)D_800A0C70[index] + ((s32)slot << 2))[4];
}
