#include "game/types.h"

struct UnkFA94 {
    u8 pad00[0x5C];
    s32 unk5C;
};

extern u8 D_8009F680;
extern s32 D_8009F9FC;
extern s32 D_8009FA00;
extern s32 D_8009FA70;
extern struct UnkFA94 *D_8009FA94;

/* Incomplete array type keeps this out of small data, so the compiler has to
   materialise the base address with lui/addiu like the retail code does. */
extern u8 *D_8009FA58[];

extern void func_80029C34(void);
extern void func_80047088(s32 arg0, void *object);
extern void func_80024EC8(void);

void func_80027D9C(void)
{
    s32 i;
    u8 *object;
    register u8 **slot __asm__("$16");

    if (D_8009F680 != 0) {
        goto tail;
    }

    if (D_8009FA00 != 4) {
        goto tail;
    }

    func_80029C34();

    if (D_8009FA70 <= 0) {
        goto tail;
    }

    D_8009FA70 = D_8009FA70 - D_8009F9FC;

    if (D_8009FA70 > 0) {
        goto tail;
    }

    D_8009FA70 = -1;

    if (D_8009FA94->unk5C == 1) {
        for (i = 0, slot = D_8009FA58; i < 2; i++) {
            object = *slot;

            if (object != 0) {
                func_80047088(0x27, object);
            }
            slot++;
        }

        D_8009FA00 = 0xD;
    } else {
        D_8009FA00 = 0xB;
    }

tail:
    func_80024EC8();
}
