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
