// Categories: Gameplay > Map | Graphics > HUD > Minimap
typedef int s32;
extern s32 gWorldClipRect[4] __asm__("D_800A2950");
/**
 * @brief Clamps a position triple to the current map bounds and reports which
 *        axes were corrected.
 *
 * @param arg0 Position triple; [0] = x, [2] = z (in place).
 * @param arg1 Margin parameter; the z margin is arg1 - 0x4000.
 * @return Bit 0 when z was clamped, bit 1 when x was clamped, 0 otherwise.
 *
 * @par Confirmed
 * z is clamped into [D_800A2950[0] + margin, D_800A2950[1] - margin]; x is
 * clamped into [D_800A2950[2] + 0x14000, D_800A2950[3] - 0x14000]. The four
 * bounds words are the active map window.
 *
 * @par Likely role
 * The camera/object leash of the minimap driver func_8006680C: keeps the
 * view inside the map with a fixed 0x14000 side margin and a configurable
 * depth margin.
 *
 * @par Unknown
 * Who updates D_800A2950 and in which units the bounds are expressed.
 *
 * @par Callers
 * func_8006680C.
 */
s32 func_800665E8(s32 *arg0, s32 arg1)
{
    s32 lower;
    register s32 first __asm__("$2");
    register s32 reserved __asm__("$3");
    register s32 x __asm__("$8");
    s32 z;
    register s32 changed __asm__("$6");
    s32 *bounds = gWorldClipRect;
    arg1 -= 0x4000;
    __asm__ volatile ("" : "=r" (reserved));
    first = bounds[0];
    z = arg0[2];
    x = arg0[0];
    __asm__("" : "+r" (first) : "r" (z), "r" (x), "r" (bounds), "r" (reserved));
    lower = first + arg1;
    changed = 0;
    if (z >= lower) goto upper_z;
    arg0[2] = lower;
    goto changed_z;
upper_z:
    arg1 = bounds[1] - arg1;
    if (arg1 >= z) goto check_x;
    arg0[2] = arg1;
changed_z:
    changed = 1;
check_x:
    lower = gWorldClipRect[2] + 0x14000;
    if (x < lower) goto changed_x;
    lower = gWorldClipRect[3] - 0x14000;
    {
        register s32 cmp __asm__("$2") = lower < x;
        __asm__("" : "+r" (cmp));
        if (!cmp) goto done;
    }
changed_x:
    arg0[0] = lower;
    changed |= 2;
done:
    return changed;
}
