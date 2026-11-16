// Categories: Platform > Streaming Audio | Platform > Sound > Voices
#include "game/types.h"

struct StreamChannel {
    s32 id;
    s32 voice;
    u8 unk08[0x1C];
};

struct StreamState {
    u8 flag;
    u8 unk01[3];
    s32 active;
    s32 unk08;
    u8 unk0C[4];
    s16 unk10;
    u8 unk12[2];
    u32 channelCount;
    u8 unk18[0x0C];
    struct StreamChannel channels[3];
};

extern struct StreamState D_800A0968;

extern void func_80022F7C(void);
extern void func_80098BF8(s32 mode);
extern void func_8002129C(void);
extern void func_80022240(s32 owner);
extern void func_80046A34(s32 handle);

/**
 * @brief Stops an active audio stream and releases its SPU and voice resources.
 *
 * Clears the active stream state in D_800A0968, tears down SPU transfer/IRQ hooks via
 * func_80022F7C and func_80098BF8(0), drains CD streaming via func_8002129C, and
 * frees each allocated stream channel's sound and voice handles.
 *
 * @param void No parameters.
 * @return Always 0.
 *
 * @par Globals
 * D_800A0968 - StreamState holding active/flag/unk08/unk10, channelCount, and channels[0..2].
 *
 * @par Control flow
 * Returns 0 early if D_800A0968.active == 0; otherwise resets stream flags, tears down SPU/CD hooks, and loops i = 0..channelCount-1 stopping non-(-1) channels.
 *
 * @par Calls
 * func_80022F7C - clears D_800A09F0 and calls func_80098A78(0); func_80098BF8 - clears SPU IRQ callback; func_8002129C - waits for CD stream idle; func_80022240 - stops channel id in D_800A07A8; func_80046A34 - releases voice handle.
 *
 * @par Side effects
 * Clears D_800A0968 active/flag/unk08/unk10, removes SPU IRQ handler, and frees stream voices.
 *
 * @par Callers
 * Confirmed: func_800226D8 (0x800226E0, audio stream shutdown sequence).
 */
s32 func_80022A64(void)
{
    u32 i;

    if (D_800A0968.active == 0) {
        return 0;
    }
    D_800A0968.unk10 = 0;
    func_80022F7C();
    func_80098BF8(0);
    D_800A0968.flag = 0;
    D_800A0968.active = 0;
    D_800A0968.unk08 = 0;
    func_8002129C();
    for (i = 0; i < D_800A0968.channelCount; i++) {
        if (D_800A0968.channels[i].id != -1) {
            func_80022240(D_800A0968.channels[i].id);
            func_80046A34(D_800A0968.channels[i].voice);
        }
    }
    return 0;
}
