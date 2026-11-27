// Categories: Platform > CD-ROM
typedef unsigned char u8;
typedef int s32;
struct CdlLOC { u8 minute, second, sector, track; };
#define BCD_TO_INT(value) (((value) >> 4) * 10 + ((value) & 0xF))
s32 gCdlLocToByteOffset(struct CdlLOC *location) __asm__("func_8001C588");


/**
 * @brief Converts a CdlLOC position back into a byte offset.
 *
 * The inverse of func_8001C494: minute, second and sector are BCD-decoded, the
 * minutes are turned into seconds, seconds into sectors (times 75) and the
 * sector/frame field is added. The result is a sector number, so returning it
 * shifted left by 11 yields the byte offset in the 2048-byte sector layout.
 *
 * @param location CdlLOC with BCD minute, second and sector bytes.
 * @return Byte offset of the position, or 0 when all fields are zero.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line BCD decode; the *5/<<4 combination is the original times-75.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8001C808, func_8001D424.
 *
 * @par Role and path
 * Decodes the BCD minute, second, and sector fields of location into a linear
 * sector index and shifts left by 11 to return the byte offset. Called by
 * func_8001C808 and func_8001D424; touches no globals and makes no calls.
 */
s32 gCdlLocToByteOffset(struct CdlLOC *location)
{
    register unsigned int m __asm__("$3");
    register s32 first __asm__("$5");
    register s32 n __asm__("$6");
    register unsigned int s __asm__("$5");
    register unsigned int f __asm__("$4");
    register s32 second __asm__("$5");
    register s32 times5 __asm__("$2");
    register s32 temp __asm__("$3");
    m = location->minute;
    __asm__ volatile ("" : "+r" (m));
    first = m >> 4;
    __asm__ volatile ("" : "+r" (first));
    first = first * 10 + (m & 15);
    __asm__ volatile ("" : "+r" (first));
    n = first * 60;
    __asm__ volatile ("" : "+r" (n) : : "memory");
    s = location->second;
    f = location->sector;
    __asm__ volatile ("" : "+r" (s), "+r" (f));
    second = n + BCD_TO_INT(s);
    __asm__ volatile ("" : "+r" (second));
    times5 = second * 5;
    __asm__ volatile ("" : "+r" (times5));
    temp = times5 << 4;
    __asm__ volatile ("" : "+r" (temp));
    second = temp - times5;
    __asm__ volatile ("" : "+r" (second));
    second = second + BCD_TO_INT(f);
    __asm__ volatile ("" : "+r" (second));
    return second << 11;
}
