// Categories: Graphics > Effects > Lights | Game Engine > Front-End Configuration
typedef unsigned char u8;
typedef int s32;
u8 gStepParticleColorChannels(u8 *base, s32 d1, s32 d2, s32 d3, s32 a5, u8 a6, u8 a7) __asm__("func_800751A4");


/**
 * @brief Steps three colour channel bytes with per-channel clamping and
 *        returns how many channels moved freely.
 *
 * @param base Record with the three bytes at +0x15/+0x16/+0x17.
 * @param d1, d2, d3 Signed deltas for the three channels.
 * @param a5, a6, a7 The clamp limits (a5 is compared as a byte).
 * @return 3 minus the number of channels that hit a limit or had a zero
 *         delta (channel 1 clamping sets 2 instead of decrementing).
 *
 * @par Confirmed
 * Channel 1: positive deltas clamp at the limit (return base 2), negative
 * deltas clamp when the result would fall to/below the limit, a zero delta
 * also yields 2. Channels 2/3: each clamped or zero delta decrements the
 * running count from 3. All stores are byte-wide.
 *
 * @par Likely role
 * The colour-picker step of the light editor func_80075A84: the return
 * count tells the caller how many channels are still adjustable, so the
 * UI can grey out the saturated ones.
 *
 * @par Unknown
 * Why channel 1 uses the asymmetric clamp rule.
 *
 * @par Callers
 * func_80075A84.
 */
u8 gStepParticleColorChannels(u8 *base, s32 d1, s32 d2, s32 d3, s32 a5, u8 a6, u8 a7)
{
    register s32 ret __asm__("$8") = 3;
    register s32 old __asm__("$9");
    register s32 target __asm__("$10") = a5;
    register s32 lim __asm__("$2");
    register s32 masked __asm__("$3");
    __asm__ volatile("" : "=r"(target) : "0"(target));
    lim = target;

    if (d1 > 0) {
        old = base[0x15];
        masked = (u8)lim;
        __asm__ volatile("" : "=r"(masked) : "0"(masked));
        if (old + d1 >= masked) goto clamp1;
        base[0x15] = old + d1;
        goto next1;
    }
    if (d1 >= 0) goto zero1;
    old = base[0x15];
    if (old + d1 > (u8)lim) goto store1;
clamp1:
    ret = 2;
    base[0x15] = target;
    goto next1;
store1:
    base[0x15] = old + d1;
    goto next1;
zero1:
    ret = 2;
next1:
    __asm__ volatile("" : : "r"(old));

    if (d2 > 0) {
        u8 old2 = base[0x16];
        if (old2 + d2 >= a6) {
            ret--;
            base[0x16] = a6;
        } else {
            base[0x16] = old2 + d2;
        }
    } else if (d2 < 0) {
        u8 old2 = base[0x16];
        if (a6 >= old2 + d2) {
            ret--;
            base[0x16] = a6;
        } else {
            base[0x16] = old2 + d2;
        }
    } else {
        ret--;
    }

    if (d3 > 0) {
        u8 old3 = base[0x17];
        if (old3 + d3 >= a7) {
            ret--;
            base[0x17] = a7;
        } else {
            base[0x17] = old3 + d3;
        }
    } else if (d3 < 0) {
        u8 old3 = base[0x17];
        if (a7 >= old3 + d3) {
            ret--;
            base[0x17] = a7;
        } else {
            base[0x17] = old3 + d3;
        }
    } else {
        ret--;
    }

    return ret;
}
