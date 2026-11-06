#include "game/types.h"

struct Item {
    s32 values[8];
    s32 other[8];
    s32 count;
};
struct Manager {
    struct Item *items[8];
    s32 count;
};
extern struct Manager *D_8009F240;
extern s32 func_8001CE10(void);
extern s32 func_8001D424(s32 arg0, s32 arg1, s32 arg2);

s32 func_8001D5E4(s32 arg0, s32 arg1)
{
    s32 result = -5;
    s32 i;
    s32 count;
    struct Item *item;
    s32 ret;

    if (func_8001CE10() < 0)
        return -2;
    if (arg1 >= D_8009F240->count || arg1 < 0)
        return -8;
    item = D_8009F240->items[arg1];
    count = item->count;
    for (i = 0; i < count; i++) {
        ret = func_8001D424(arg0, item->values[i], item->other[i]);
        if (ret >= 0)
            return ret;
        if ((u32)(ret + 6) >= 2)
            result = ret;
    }
    return result;
}
