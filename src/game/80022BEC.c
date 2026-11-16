// Categories: Resources > Buffer Management
typedef unsigned int u32;
typedef int s32;
struct Data {
    char pad0[0x14];
    u32 count;
    char pad18[0x54];
    s32 width;
    s32 height;
};
extern struct Data D_800A0968;
extern volatile s32 D_8009F27C;

/**
 *  @brief Assigns sequential buffer addresses to entries in a global table.
 *
 *  Each address advances by the fixed width times height from the table.
 *  A zero base or a negative computed address stops assignment immediately.
 *  @return Zero on completion, or the first negative address value encountered.
 *  @par Globals
 *  D_800A0968 supplies the count and dimensions and receives entry addresses;
 *  D_8009F27C supplies the volatile base address.
 *  @par Control flow
 *  Iterates over count entries at a 0x24-byte stride, returning early on a negative value.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes nonnegative addresses at offset 0x44 of successive table entries.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80022BEC(void)
{
    register s32 offset __asm__("$7") = 0;
    register u32 i __asm__("$6") = offset;
    register s32 result __asm__("$9") = i;
    s32 value;
    u32 count;
    register s32 base __asm__("$8") = D_8009F27C;
    register u32 high __asm__("$2") = 0x800A0000;
    register struct Data *ptr __asm__("$3");
    register struct Data *fixed __asm__("$5");
    register char *entry __asm__("$4");
    __asm__("" : "+r" (high));
    ptr = (struct Data *)(high + 0x0968);
    if (ptr->count != 0) {
        fixed = ptr;
        __asm__("" : "+r" (fixed));
        entry = (char *)fixed;
        __asm__("" : "+r" (entry));
        do {
            value = -1;
            if (base != 0) {
                value = base + offset;
                offset += fixed->width * fixed->height;
            }
            if (value < 0)
                return value;
            *(s32 *)(entry + 0x44) = value;
            count = fixed->count;
            __asm__("" : "+r" (i) : "r" (count));
            i++;
            entry += 0x24;
        } while (i < count);
    }
    return result;
}
