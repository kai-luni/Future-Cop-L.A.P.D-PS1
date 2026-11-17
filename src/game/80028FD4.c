// Categories: Graphics > Fade Layers | Utilities > Mathematics
typedef unsigned short u16;

extern u16 gStreamBufferList[] __asm__("D_800A0B70");

/**
 * @brief Returns the ratio of two u16 counters scaled by 1024.
 *
 * @return (D_800A0B70[0x1F] << 10) / D_800A0B70[0x20], or 0 when the
 *         denominator is zero.
 *
 * @par Confirmed
 * Guarded integer division, no rounding.
 *
 * @par Likely role
 * Fade progress in 1024 steps: the caller is the fade-quad effect, the
 * pair reads as current/total step counters.
 *
 * @par Callers
 * func_80025690.
 */
int func_80028FD4(void)
{
    if (gStreamBufferList[0x20] == 0) {
        return 0;
    }
    return (gStreamBufferList[0x1F] << 10) / gStreamBufferList[0x20];
}
