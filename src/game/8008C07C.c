typedef unsigned char u8;

typedef struct {
    u8 pad00[0x28];
    u8 *data0;
    u8 *data1;
    u8 pad30[4];
    u8 count0;
    u8 count1;
    u8 mode;
    u8 pad37[0xe];
    u8 index;
    u8 pad46[0x11];
    u8 enabled[6];
} Object;

int func_8008C07C(Object *obj)
{
    int index = obj->index - 3;
    volatile u8 *p;
    switch (obj->mode) {
    case 0:
        if (index < 6 && !obj->enabled[index]) return 0;
        if (index < obj->count0) {
            p = obj->data0 + index;
load:
            return *p;
        }
        return 0;
    case 0x4d:
        if (index < obj->count1) {
            p = obj->data1 + index;
            goto load;
        }
        return 0xff;
    default:
        if (index < obj->count1) return obj->data1[index];
        return 0;
    }
}
