#include "game/types.h"
typedef struct { u8 unk00[4]; u8 count; u8 unk05[0xB]; s32 offsets[1]; } D;

void func_800219EC(u8 *arg0, s32 amount)
{
    s32 i; register s32 j __asm__("$6");
    D *data = (D *)arg0;
    s32 *offsets = data->offsets;
    for (i = 0; i < data->count; i++) {
        s32 offset = offsets[i];
        if (offset != 0) {
            u8 *entry = arg0 + offset;
            s32 *p = (s32 *)entry;
            for (j = 0; j < *entry; j++) { p[5] += amount; p += 5; }
        }
    }
}
