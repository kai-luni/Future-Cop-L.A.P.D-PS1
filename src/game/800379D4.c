#include "game/types.h"

typedef struct Obj379D4 {
    u8 pad00[0x1C];
    s16 val1C;
    u8 pad1E[0x24 - 0x1E];
    s32 val24;
    u8 pad28[0x84 - 0x28];
    s32 val84;
    u8 pad88[0xF8 - 0x88];
    s32 posF8;
    s16 posFC;
    u8 padFE[0x10C - 0xFE];
    s16 val10C;
    u8 pad10E[0x118 - 0x10E];
    u32 flags;
    s16 val11C;
    u8 pad11E[2];
    s16 val120;
    u8 pad122[2];
    s16 val124;
} Obj379D4;

extern s32 func_8002AD58(s32 message, Obj379D4 *object, void *data);

void func_800379D4(s32 message, Obj379D4 *object, Obj379D4 *data)
{
    register s32 t2 __asm__("$10");
    register u32 flags __asm__("$9");
    register Obj379D4 *other __asm__("$8");
    register Obj379D4 *temp __asm__("$2");
    register s32 notify __asm__("$2");
    s32 dData;
    s32 dObj;
    s32 half;
    s32 sum;
    s32 range;

    flags = object->flags;
    t2 = message;
    if ((flags >> 30) == 1) {
        object->val1C = -1;
        object->flags |= 0xC0000000;
    }
    other = data;
    if ((flags & 0x8) != 0) {
        goto NOTIFY;
    }
    if (data->val24 >= 0) {
        goto NOTIFY;
    }
    if ((flags & 0x40) != 0) {
        goto COND;
    }
    if (data->val11C < 0) {
        goto COND;
    }
    if (data->val120 == object->val120) {
        dData = (data->posFC << 8) - data->posF8;
        dObj = (object->posFC << 8) - object->posF8;
        if (dData < dObj) {
            goto SWAP;
        }
        goto UPDATE;
    }
    if (data->val124 != object->val120) {
        goto CHECK;
    }
SWAP:
    temp = other;
    other = object;
    object = temp;
CHECK:
    if (other->val120 == object->val120) {
        goto UPDATE;
    }
    if (object->val124 != other->val120) {
        goto COND;
    }
UPDATE:
    half = other->val84 >> 1;
    sum = object->val84 + half;
    range = object->val10C << 1;
    object->val84 = sum;
    if (range < sum) {
        object->val84 = range;
    }
    object->val11C = -300;
    other->val11C = 300;
    other->val84 = other->val84 >> 1;
COND:
    notify = flags & 0x80;
    if (notify != 0) {
      NOTIFY:
        func_8002AD58(t2, object, data);
    }
}
