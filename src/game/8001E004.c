// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

typedef struct {
    volatile s32 field0;
    u8 unk04[4];
    s32 field8;
    u8 unk0C[0xC];
} Unk800A6D78;

extern Unk800A6D78 D_800A6D78[];
extern volatile u8 D_8009FA68;
extern s32 D_8009F620;

extern s32 func_8001D90C(s32 index, s32 arg1, s32 value, u32 (*callback)(u32));
extern s32 func_8001D3A8(u32 index);
extern u32 func_8001DFF0(u32 arg0);

/**
 * @brief Reads the contents of an open file into a buffer and closes it.
 *
 * @param arg0 Slot index 0..3.
 * @param arg1 Destination buffer; NULL returns -0x11.
 * @param arg2 Optional out-parameter receiving the number of bytes read.
 *
 * @return The number of bytes read (the value the asynchronous callback stored
 *         in D_8009F620), or a negative error code (-9 bad index, -0x11 no
 *         buffer, or whatever func_8001D90C reported).
 *
 * @par Confirmed flow
 * 1. The slot's state is validated and D_8009FA68 (completion flag) and
 *    D_8009F620 (byte count) are cleared.
 * 2. func_8001D90C(index, buffer, size, func_8001DFF0) starts the read; its
 *    callback is func_8001DFF0, which sets D_8009FA68.
 * 3. The function spins while D_8009FA68 == 0 - a blocking read on top of the
 *    asynchronous primitive.
 * 4. The byte count is copied out, func_8001D3A8(index) closes the file and, if
 *    requested, *arg2 receives the count.
 *
 * @par Globals
 * D_800A6D78 (file slots), D_8009FA68 (completion flag), D_8009F620 (byte
 * count).
 *
 * @par Callers
 * func_8001E110 only.
 */
s32 func_8001E004(s32 arg0, s32 arg1, s32 *arg2)
{
    s32 value;
    s32 arg;
    s32 result;
    s32 stored;

    if (arg1 == 0) {
        return -0x11;
    }

    value = -9;
    if ((u32)arg0 < 4) {
        if (D_800A6D78[arg0].field0 != 0) {
            value = D_800A6D78[arg0].field0;
        }
    }
    if (value < 0) {
        return value;
    }

    D_8009FA68 = 0;
    D_8009F620 = 0;

    arg = -9;
    if ((u32)arg0 < 4) {
        if (D_800A6D78[arg0].field0 != 0) {
            arg = D_800A6D78[arg0].field8;
        }
    }

    value = func_8001D90C(arg0, arg1, arg, func_8001DFF0);
    if (value < 0) {
        return value;
    }

    while (D_8009FA68 == 0) {
    }

    stored = D_8009F620;
    func_8001D3A8(arg0);
    if (arg2 != 0) {
        *arg2 = stored;
    }
    return stored;
}
