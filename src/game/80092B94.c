// Categories: Platform > MDEC | Graphics > Video Decode
#include "game/types.h"

typedef void (*DecDctCallback)(void);
typedef void (*DecDctFn)(u8 *, DecDctCallback);

extern  void (*gLogPrintf)(u8 *, DecDctCallback) __asm__("D_8009DDEC");
extern u8 D_80012A14[];
extern u8 gGpuDebugLevel[] __asm__("D_8009DDF2");

/**
 * @brief Gets or sets the decoder's completion callback.
 *
 * @param callback New callback (may be NULL).
 * @return The previously installed callback.
 *
 * @par Confirmed flow
 * With debug level D_8009DDF2[0] >= 2 the hook is called with D_80012A14, then
 * the old pointer at context + 0xA is read, replaced by @c callback and
 * returned.
 *
 * @par Globals
 * D_8009DDF2 (library state, is a byte array here so the debug level sits at
 * offset 0), D_8009DDEC (debug hook), D_80012A14 (debug string).
 *
 * @par Likely role
 * Part of the Sony MDEC interface (the DecDct* family): the three slots filled by
 * func_800861E4 are the frame callback (func_8009612C), this one and
 * func_80096108.
 *
 * @par Callers
 * func_800861E4, with NULL, twice per job (set-up and cleanup).
 */
DecDctCallback func_80092B94(DecDctCallback callback)
{
    u8 *p = gGpuDebugLevel;
    DecDctCallback old;
    __asm__("" : "=r"(p) : "0"(p));
    if ((u32)p[0] >= 2) {
        DecDctFn fn = gLogPrintf;
        fn(D_80012A14, callback);
    }
    old = *(DecDctCallback *)(p + 0xA);
    *(DecDctCallback *)(p + 0xA) = callback;
    return old;
}
