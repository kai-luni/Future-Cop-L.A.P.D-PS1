#include "game/types.h"

typedef void (*DecDctCallback)(void);
typedef void (*DecDctFn)(u8 *, DecDctCallback);

extern void (*D_8009DDEC)(u8 *, DecDctCallback);
extern u8 D_80012A14[];
extern u8 D_8009DDF2[];

DecDctCallback func_80092B94(DecDctCallback callback)
{
    u8 *p = D_8009DDF2;
    DecDctCallback old;
    __asm__("" : "=r"(p) : "0"(p));
    if ((u32)p[0] >= 2) {
        DecDctFn fn = D_8009DDEC;
        fn(D_80012A14, callback);
    }
    old = *(DecDctCallback *)(p + 0xA);
    *(DecDctCallback *)(p + 0xA) = callback;
    return old;
}
