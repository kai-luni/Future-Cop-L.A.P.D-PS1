// Categories: Game Engine > Object State
#include "game/types.h"

struct Unk3CDACRange {
    u32 count;
};

struct Unk3CDACObject {
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x20];
    u8 slots[8];
    struct Unk3CDACRange *range;
};

struct Unk3CDACHolder {
    struct Unk3CDACObject *object;
};

/**
 *  @brief Tests whether an indexed entry is available in a held object.
 *
 *  Flag bit 25 selects either a range-count check or a slot sentinel check.
 *  The slot path directly indexes the eight-byte array without bounds checks.
 *  @param holder Holder whose object is tested.
 *  @param index Signed entry index to test.
 *  @return 1 if the selected check succeeds, otherwise 0.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  A null object fails; the range path also rejects null ranges and negative indices.
 *  The slot path succeeds when the indexed byte is not 0xFF.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None directly; an invalid slot index can access outside the array.
 *  @par Callers
 *  Confirmed: func_8003E984.
 */
s32 func_8003CDAC(struct Unk3CDACHolder *holder, s8 index)
{
    struct Unk3CDACObject *object;
    register struct Unk3CDACRange *range __asm__("$3");
    register u32 flagWord __asm__("$2");
    register s32 result __asm__("$2");

    object = holder->object;
    result = 0;
    if (object == 0) {
        goto done;
    }
    flagWord = object->flags;
    flagWord >>= 25;
    flagWord &= 1;
    if (flagWord != 0) {
        range = object->range;
        if (range == 0) {
            goto zero;
        }
        if (index < 0) {
            goto done;
        }
        if (index < range->count) {
            result = 1;
            goto done;
        }
zero:
        result = 0;
        goto done;
    }
    if (object->slots[index] == 0xFF) {
        goto done;
    }
    result = 1;
done:
    return result;
}
