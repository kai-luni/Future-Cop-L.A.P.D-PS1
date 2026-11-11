#include "game/types.h"

extern u16 D_8009DDF4, D_8009DDF6;
extern u32 *D_8009DEC8;
extern u32 D_800A6280[], D_800A62A8[];
extern u32 func_800949E8(u32);
extern void func_800949A0(u32);

s32 func_8009423C(u16 *arg0, u32 arg1)
{
    register u16 *rect __asm__("$8") = arg0;
    register u32 color __asm__("$9") = arg1;
    register s32 result __asm__("$2");
    rect[2] = (s16)rect[2] < 0 ? 0 : ((s16)D_8009DDF4 - 1 < (s16)rect[2] ? D_8009DDF4 - 1 : rect[2]);
    rect[3] = (s16)rect[3] < 0 ? 0 : ((s16)D_8009DDF6 - 1 < (s16)rect[3] ? D_8009DDF6 - 1 : rect[3]);

    if ((rect[0] & 0x3f) != 0 || (rect[2] & 0x3f) != 0) {
        register u32 mask __asm__("$6") = 0xffffff;
        register u32 limit __asm__("$4") = 0xe4ffffff;
        register u32 tag __asm__("$7") = 0x03ff0000;
        register u32 *p __asm__("$5");
        register u32 *q __asm__("$16");
        u32 xy, header;
        __asm__ volatile ("" : "+r"(mask), "+r"(limit), "+r"(tag));
        p = D_800A6280;
        q = D_800A62A8;
        __asm__ volatile ("" : "+r"(p), "+r"(q));
        p[0] = ((u32)q & mask) | 0x08000000;
        p[1] = 0xe3000000;
        p[2] = limit;
        p[3] = 0xe5000000;
        p[4] = 0xe6000000;
        header = 0xe1000000 | ((color >> 31) << 10);
        p[5] = (*D_8009DEC8 & 0x7ff) | header;
        p[6] = 0x60000000 | (color & mask);
        __asm__ volatile ("" : : "r"(color));
        tag |= 0xffff;
        p[7] = *(u32 *)&rect[0];
        xy = *(u32 *)&rect[2];
        q[0] = tag;
        p[8] = xy;
        q[1] = 0xe3000000 | func_800949E8(3);
        q[2] = 0xe4000000 | func_800949E8(4);
        q[3] = 0xe5000000 | func_800949E8(5);
    } else {
        register u32 value __asm__("$2") = 0x05ffffff;
        register u32 mask __asm__("$3") = 0x00ffffff;
        register u32 *p __asm__("$6");
        u32 gpu;
        register u32 masked __asm__("$4");
        register u32 mode __asm__("$2");
        u32 rgb;
        __asm__ volatile ("" : "+r"(value), "+r"(mask));
        p = D_800A6280;
        __asm__ volatile ("" : "+r"(p));
        p[0] = value;
        p[1] = 0xe6000000;
        rgb = 0x02000000 | (color & mask);
        gpu = *D_8009DEC8;
        __asm__("" : "+r"(color) : "r"(gpu));
        mode = (color >> 31) << 10;
        p[3] = rgb;
        __asm__ volatile ("" : "+r"(mode));
        mode |= 0xe1000000;
        __asm__("" : "+r"(mode));
        masked = gpu & 0x7ff;
        __asm__("" : "+r"(masked));
        masked |= mode;
        __asm__("" : "+r"(masked));
        p[2] = masked;
        __asm__ volatile ("" : : "r"(color));
        p[4] = *(u32 *)&rect[0];
        p[5] = *(u32 *)&rect[2];
    }
    func_800949A0((u32)D_800A6280);
    result = 0;
    __asm__ volatile ("" : "+r"(result));
    return result;
}
