// Categories: Graphics > MDEC > Decode Callbacks | Platform > MDEC
#include "game/types.h"

typedef void (*DecDctCallback)(void);

extern DecDctCallback func_800914E0(s32 channel, DecDctCallback callback);

/**
 * @brief Registers a callback for MDEC channel 0.
 *
 * @param callback Function pointer, or NULL to clear.
 * @return The previous callback (as returned by func_800914E0).
 *
 * @par Confirmed
 * func_800914E0(0, callback); func_8009612C is the same for channel 1.
 *
 * @par Likely role
 * The two decode channels of the MDEC stream: one for the bitstream, one for the
 * finished frame.
 */
DecDctCallback func_80096108(DecDctCallback callback)
{
    return func_800914E0(0, callback);
}
