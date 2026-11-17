// Categories: Resources > File Access | Platform > CD-ROM
#include "game/types.h"

typedef struct {
    volatile s32 field0;
    u8 unk04[4];
    s32 field8;
    u8 unk0C[0xC];
} Unk800A6D78;

extern Unk800A6D78 D_800A6D78[];
extern s32 D_8009F258[];
extern  s32 (*gAllocCallback)(s32, s32) __asm__("D_8009F230");

extern s32 func_8001D424(s32 arg0, s32 arg1, void *arg2);
extern s32 func_8001D5E4(s32 arg0, s32 arg1);
extern s32 func_8001E004(s32 arg0, s32 arg1, s32 *arg2);

/**
 * @brief Loads a whole file into a newly allocated buffer.
 *
 * @param arg0 File name/index understood by func_8001D5E4 (positive path) or
 *             func_8001D424 (negative path).
 * @param arg1 Selects the lookup: >= 0 looks the name up with func_8001D5E4;
 *             < 0 negates the value and looks it up with func_8001D424 against
 *             the alternate table D_8009F258.
 * @param arg2 Out: pointer to the allocated data (NULL for an empty file).
 * @param arg3 Out: size in bytes as reported by the loader (0 for empty).
 * @param arg4 Forwarded to the allocation callback D_8009F230 as its second
 *             argument (allocation mode/tag).
 *
 * @return 0 on success (including "file exists but is empty"), -5 when the
 *         index is out of range or the file is not open, -0x16 when the
 *         allocation callback returned NULL. Otherwise the result of
 *         func_8001E004().
 *
 * @par Confirmed flow
 * 1. index = func_8001D5E4(name, size) (or func_8001D424 for the negative
 *    case) - a lookup in the four-slot file table D_800A6D78.
 * 2. index >= 4, or field0 == 0 (slot not open) => -5.
 * 3. Slot open but field8 (size) == 0 => both out-parameters are cleared and
 *    0 is returned: an empty file is not an error.
 * 4. Otherwise the slot size is passed to the installed allocator callback
 *    D_8009F230(size, arg4); NULL means -0x16.
 * 5. func_8001E004(index, buffer, &size) reads the data and fills the size
 *    out-parameter; its return value is passed through.
 *
 * @par Globals
 * D_800A6D78 (four file slots: volatile flag, size, ...), D_8009F258 (alternate
 * lookup table), D_8009F230 (allocation callback installed by func_8001D248).
 *
 * @par Callers
 * func_80025370 (loading-screen *.lsc artwork) and func_80059BA0.
 *
 * @par Unknown
 * The exact error-code numbering (-5, -0x16) - it matches neither the Sony
 * library's usual codes nor the BIOS file API one-to-one.
 */
s32 func_8001E110(s32 arg0, s32 arg1, s32 *arg2, s32 *arg3, s32 arg4)
{
    s32 index;
    s32 value;
    s32 ret;

    if (arg1 < 0) {
        arg1 = -arg1;
        index = func_8001D424(arg0, arg1, D_8009F258);
    } else {
        index = func_8001D5E4(arg0, arg1);
    }

    if ((u32)index >= 4) {
        return -5;
    }
    if (D_800A6D78[index].field0 == 0) {
        return -5;
    }
    if (D_800A6D78[index].field0 != 0 && D_800A6D78[index].field8 == 0) {
        *arg3 = 0;
        *arg2 = 0;
        return 0;
    }

    value = -9;
    if ((u32)index < 4) {
        if (D_800A6D78[index].field0 != 0) {
            value = D_800A6D78[index].field8;
        }
    }

    ret = gAllocCallback(value, arg4);
    *arg2 = ret;
    if (ret == 0) {
        return -0x16;
    }
    return func_8001E004(index, ret, arg3);
}
