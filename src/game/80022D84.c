#include "game/types.h"

struct StreamChannel {
    s32 id;
    s32 voice;
    s32 left;
    s32 right;
    s32 start;
    s32 length;
    u8 looping;
    u8 pad19[7];
    s32 unk20;
};

struct StreamState {
    u8 flag;
    u8 pad01[3];
    s32 active;
    s32 unk08;
    s32 stopping;
    u16 unk10;
    u8 pad12[14];
    s32 unk20;
    struct StreamChannel channels[2];
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    volatile s32 ready;
};

struct Request {
    s32 address;
    u32 size;
    void *data;
    void (*callback)(void);
    u32 unused;
    s32 mode;
};

extern struct StreamState D_800A0968;

extern void func_80022D1C(void *data, s32 mode, s32 length);
extern void func_80023204(void);
extern void func_800212F8(struct Request *req);

void func_80022D84(void)
{
    struct Request req;
    s32 mode;
    s32 channel;

    channel = D_800A0968.unk20;
    if (channel == 0) {
        if (D_800A0968.channels[0].start == 0 || D_800A0968.unk74 == 0) {
            return;
        }
    }

    if (D_800A0968.unk08 != 0) {
        if (channel == 0) {
            if (D_800A0968.unk74 == D_800A0968.unk70) {
                D_800A0968.unk78 = D_800A0968.unk7C + D_800A0968.unk6C;
                if (D_800A0968.unk78 == D_800A0968.unk74 * D_800A0968.unk6C) {
                    D_800A0968.unk78 = 0;
                }
                D_800A0968.unk74--;
            }
        }
    }

    D_800A0968.ready = 0;
    if (channel == 0) {
        D_800A0968.unk74--;
    }

    if (D_800A0968.channels[channel].looping != 0) {
        mode = 3;
    } else if (D_800A0968.unk78 == 0) {
        mode = 0;
    } else if (D_800A0968.unk78 == (D_800A0968.unk70 - 1) * D_800A0968.unk6C) {
        mode = 1;
    } else {
        mode = 2;
    }

    func_80022D1C((void *)D_800A0968.channels[channel].start, mode, D_800A0968.channels[channel].length);

    req.address = D_800A0968.channels[channel].unk20 + D_800A0968.unk78;
    req.size = D_800A0968.channels[channel].length;
    req.data = (void *)D_800A0968.channels[channel].start;
    req.callback = func_80023204;
    req.unused = D_800A0968.unk10;
    if (mode == 3) {
        req.unused |= 0x80000000;
    }
    req.mode = 1;

    func_800212F8(&req);
}
