// Categories: Graphics > MDEC > Decode Callbacks | Platform > MDEC
#include "game/types.h"

typedef void (*DecDctCallback)(void);

extern DecDctCallback gSetDmaCallback(s32 channel, DecDctCallback callback) __asm__("func_800914E0");

/**
 * @brief Registers a callback for MDEC channel 1.
 *
 * @param callback Function pointer, or NULL to clear.
 * @return The previous callback (as returned by func_800914E0).
 *
 * @par Confirmed
 * func_800914E0(1, callback); pair of func_80096108 (channel 0).
 */
DecDctCallback func_8009612C(DecDctCallback callback)
{
    return gSetDmaCallback(1, callback);
}
