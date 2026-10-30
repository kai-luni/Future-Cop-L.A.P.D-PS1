#include "game/types.h"

extern void *D_8009F948;
extern void *D_8009F94C;
extern void *D_8009F950;
extern void *D_8009F954;

extern void func_800181D4(void *block);

void func_80086164(void)
{
    if (D_8009F948) {
        func_800181D4(D_8009F948);
    }
    if (D_8009F94C) {
        func_800181D4(D_8009F94C);
    }
    if (D_8009F950) {
        func_800181D4(D_8009F950);
    }
    if (D_8009F954) {
        func_800181D4(D_8009F954);
    }
    D_8009F948 = (void *)0;
    D_8009F94C = (void *)0;
    D_8009F950 = (void *)0;
    D_8009F954 = (void *)0;
}
