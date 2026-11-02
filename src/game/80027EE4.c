#include "game/types.h"

extern s32 D_8009F284;
extern s32 D_8009F9FC;
extern s32 D_8009FA00;
extern s32 D_8009FA30;
extern s32 D_8009FA50;
extern volatile s32 D_8009FA60;
extern s32 D_8009FAA4;

extern void func_80027E68(void);
extern void func_80027EBC(void);
extern void func_800451EC(s32 value);
extern void func_800462D0(void);
extern void func_80078DD0(void);

void func_80027EE4(void)
{
    s32 t;
    D_8009FA60 = 0;
    t = D_8009FA60;
    D_8009F9FC = 8;
    D_8009FAA4 = t;
    D_8009FA30 = t;
    D_8009FA50 = 0;

    while (D_8009FA00 == 4) {
        D_8009FA50++;
        D_8009F284++;
        func_80027E68();
        func_800451EC(0);
        func_800462D0();
        func_80078DD0();
        func_80027EBC();
    }
}
