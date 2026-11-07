typedef int s32;
extern s32 D_800A2950[4];
s32 func_800665E8(s32 *arg0, s32 arg1)
{
    s32 lower;
    register s32 first __asm__("$2");
    register s32 reserved __asm__("$3");
    register s32 x __asm__("$8");
    s32 z;
    register s32 changed __asm__("$6");
    s32 *bounds = D_800A2950;
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
    lower = D_800A2950[2] + 0x14000;
    if (x < lower) goto changed_x;
    lower = D_800A2950[3] - 0x14000;
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
