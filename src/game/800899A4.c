#include "game/types.h"

typedef struct Unk800899A4 {
    u16 *unk0;
    u8 unk04[0x18];
    s32 unk1C;
} Unk800899A4;

extern Unk800899A4 *D_8009F518;
extern void func_800181D4(Unk800899A4 *arg0);

void func_800899A4(Unk800899A4 *arg0)
{
    s32 temp = arg0->unk1C;
    s32 flag = 0;
    s32 id;

    if (temp != 1) {
        func_800181D4(arg0);
        return;
    }
    if (D_8009F518 != 0) {
        func_800181D4(D_8009F518);
    }
    id = arg0->unk0[0];
    D_8009F518 = arg0;
    if (id != temp) {
        flag = 1;
    }
    if (flag != 0) {
        func_800181D4(arg0);
        D_8009F518 = 0;
    }
}
