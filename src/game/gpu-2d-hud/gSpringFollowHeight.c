// Categories: Graphics > Camera > Smoothing
typedef int s32;
typedef unsigned int u32;
#define FMUL(d,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
s32 gSpringFollowHeight(s32 *obj, s32 scale, s32 count) __asm__("func_80056530");


/**
 * @brief Spring-eases a camera offset word toward the object's value with a
 *        cubic impulse and a dead zone.
 *
 * @param obj Follow object; +0x54 the target value, +0xAC the spring state.
 * @param scale Output scale.
 * @param count Integration sub-steps.
 * @return The new state value (+0xD4).
 *
 * @par Confirmed
 * delta = target - current; the impulse is (delta << 3)^3 in 16.16 (two
 * FMULs), rounded >> 3. A sign change against the accumulator (+0xD8)
 * resets it; the impulse accumulates count times, then >> 14 * scale gives
 * the step. |step| < 256 zeroes it; |step| > |delta| clamps to delta. The
 * current value advances by the step; delta == 0 clears the accumulator.
 *
 * @par Likely role
 * The boom/offset spring of the chase camera func_800569EC: the cubic
 * impulse makes large corrections fast and small ones gentle, the dead
 * zone stops sub-pixel jitter.
 *
 * @par Callers
 * func_800569EC.
 *
 * @par Environment path
 * Spring-eases the follow height kept at +0xD4 of the camera info block (the
 * pointer at object+0xAC) toward the followed object's y at +0x54: the error
 * is scaled by 8 and cubed in 16.16 fixed point, the accumulator at +0xD8 is
 * reset on a sign change and gains the impulse 'count' times, the step is
 * (accumulator >> 14) * scale, zeroed below magnitude 256 and clamped to the
 * error, and the new value is stored at +0xD4 and returned.
 * gUpdateChaseCamera (func_800569EC) calls it once per frame with (object,
 * elapsed, speed) and stores the result as the y of its local follow position
 * (800569EC.c:261). A zero error only clears the accumulator, so the port
 * must keep +0xD4/+0xD8 per camera info block.
 */
s32 gSpringFollowHeight(s32 *obj, s32 scale, s32 count)
{
    register s32 *state __asm__("$10");
    register s32 delta __asm__("$11");
    register s32 value __asm__("$3");
    register s32 step __asm__("$7");
    register s32 current __asm__("$2");
    register s32 rounded __asm__("$4");
    register s32 magnitude __asm__("$3");
    state = *(s32 **)((char *)obj + 0xac);
    value = obj[0x54 / 4];
    current = state[0xd4 / 4];
    delta = value - current;
    if (delta != 0) {
        value = delta << 3;
        FMUL(step, value, value);
        FMUL(value, step, value);
        rounded = value;
        if (value < 0) rounded = value + 7;
        value = rounded >> 3;
        if ((value ^ state[0xd8 / 4]) < 0)
            state[0xd8 / 4] = 0;
        while (count > 0) {
            state[0xd8 / 4] += value;
            count--;
        }
        current = state[0xd8 / 4];
        current >>= 14;
        step = current * scale;
        if (((step >> 31) + step ^ (step >> 31)) < 256) {
            step = 0;
            __asm__("" : "+r"(step));
        }
        magnitude = ((step >> 31) + step) ^ (step >> 31);
        __asm__("" : "+r"(magnitude));
        if (((delta >> 31) + delta ^ (delta >> 31)) < magnitude)
            step = delta;
        current = state[0xd4 / 4];
        value = current + step;
    } else {
        __asm__("" : "+r"(delta));
        state[0xd8 / 4] = 0;
    }
    state[0xd4 / 4] = value;
    return value;
}
