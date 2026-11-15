// Categories: Platform > CD-ROM | Resources > File Access
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

/**
 * @brief Opens a file by trying every directory of one search group.
 *
 * @param arg0 File name/index for func_8001D424.
 * @param arg1 Index into the search groups of D_8009F240 (must be
 *             0 <= arg1 < count).
 *
 * @return The slot index from a successful func_8001D424, -2 (device not
 *         ready), -8 (group index out of range) or -5 when no directory
 *         matched.
 *
 * @par Confirmed flow
 * Each group D_8009F240->items[arg1] holds up to eight (name, buffer) pairs and
 * a count; the function tries func_8001D424(arg0, name[i], buffer[i]) for each
 * of them and returns the first non-negative result. A failure that is not
 * -6 or -7 is remembered in @c result and returned when the whole group fails,
 * so the last "real" error survives instead of a generic one.
 *
 * @par Globals
 * D_8009F240 (search-group manager, filled by the file layer).
 *
 * @par Callers
 * func_8001E110 (file loading) and func_80088120 (video resources).
 */
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
