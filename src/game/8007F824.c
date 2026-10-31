#include "game/types.h"

extern void func_8007F688(u8 *data, u32 len, u32 *sum, u32 *other);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);

s32 func_8007F824(u8 *data, u32 len, u32 expectedSum, u32 expectedOther, s32 clear)
{
    u32 sum;
    u32 other;

    func_8007F688(data, len, &sum, &other);
    if (expectedSum != sum || expectedOther != other) {
        if (clear != 0) {
            func_8001E4C0(data, 0, len);
        }
        return 0;
    }

    return 1;
}
