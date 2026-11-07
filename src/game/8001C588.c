typedef unsigned char u8;
typedef int s32;
struct CdlLOC { u8 minute, second, sector, track; };
#define BCD_TO_INT(value) (((value) >> 4) * 10 + ((value) & 0xF))
s32 func_8001C588(struct CdlLOC *location)
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
