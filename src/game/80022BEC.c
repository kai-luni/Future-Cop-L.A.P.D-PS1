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
extern struct Data gMixerState __asm__("D_800A0968");
extern volatile s32 gVolatileBase __asm__("D_8009F27C");

s32 gAssignAudioStreamSpuBuffers(void) __asm__("func_80022BEC");

/**
 * @brief Assigns consecutive SPU buffer addresses to the configured stream channels.
 *
 * @param void No parameters.
 * @return Zero on completion, or the first negative address value encountered.
 *
 * @par Confirmed mechanics
 * Loads the base and count once at entry. Each record receives base plus cumulative block size
 * times block count at +0x44. The delay-slot store also writes the first negative result before
 * returning it.
 *
 * @par Globals
 * Reads gVolatileBase and the gMixerState count and dimensions, and writes addresses into the
 * mixer channel records.
 *
 * @par Callers
 * Direct calls from gConfigureAudioStreaming (func_80022484).
 *
 * @par Role and path
 * Stream initialization partitions the SPU buffer before priming playback.
 *
 * @par Unknown
 * The code treats a zero base as -1 and terminates after writing the failed slot.
 */
s32 gAssignAudioStreamSpuBuffers(void)
{
    register s32 offset __asm__("$7") = 0;
    register u32 i __asm__("$6") = offset;
    register s32 result __asm__("$9") = i;
    s32 value;
    u32 count;
    register s32 base __asm__("$8") = gVolatileBase;
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
