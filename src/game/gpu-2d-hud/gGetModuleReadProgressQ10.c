// Categories: Graphics > Fade Layers | Utilities > Mathematics
typedef unsigned short u16;

extern u16 gStreamBufferList[] __asm__("D_800A0B70");

int gGetModuleReadProgressQ10(void) __asm__("func_80028FD4");

/**
 * @brief Returns the module buffer progress as a ratio scaled by 1024.
 *
 * @param void No parameters.
 * @return (D_800A0B70[0x1F] << 10) / D_800A0B70[0x20], or 0 when the denominator is zero.
 *
 * @par Confirmed mechanics
 * Loads the unsigned halfwords at stream state +0x3E and +0x40. Returns zero for a zero
 * denominator, otherwise shifts the first left by ten and divides by the second.
 *
 * @par Globals
 * Reads the two progress halfwords in gStreamBufferList.
 *
 * @par Callers
 * Direct calls from gDrawScreenFadeQuad (func_80025690).
 *
 * @par Role and path
 * The fade-quad renderer uses this progress ratio during module loading.
 *
 * @par Unknown
 * The ratio is not saturated at 1024.
 */
int gGetModuleReadProgressQ10(void)
{
    if (gStreamBufferList[0x20] == 0) {
        return 0;
    }
    return (gStreamBufferList[0x1F] << 10) / gStreamBufferList[0x20];
}
