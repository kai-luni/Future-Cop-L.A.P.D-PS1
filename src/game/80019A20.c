#include "game/types.h"

typedef struct Unk80019A20 {
    s32 id;
    s32 offset;
    u8 unk08[4];
} Unk80019A20;

void *func_80019A20(s32 id, u8 *base, s32 size)
{
    u8 *current = base;
    u8 *found;
    void *result;

    do {
        found = current + 0xC;
        if (id == ((Unk80019A20 *)current)->id) {
            goto matched;
        }
        current += ((Unk80019A20 *)current)->offset;
    } while ((current - base) < size);

    result = 0;
    goto done;

matched:
    result = found;

done:
    return result;
}
