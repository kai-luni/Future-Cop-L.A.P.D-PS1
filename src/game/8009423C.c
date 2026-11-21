// Categories: Graphics > GPU Commands
#include "game/gpu_globals.h"

#include "game/types.h"

extern u16 gMaxDrawHeight __asm__("D_8009DDF6");
extern u16 gMaxDrawWidth __asm__("D_8009DDF4");
extern u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 gGpuPacketArrayAlt[] __asm__("D_800A62A8");
extern u32 gGpuPacketArray[] __asm__("D_800A6280");
extern u32 func_800949E8(u32);
extern void func_800949A0(u32);
s32 gSubmitGpuRectangleFill(u16 *arg0, u32 arg1) __asm__("func_8009423C");


/**
 *  @brief Builds and submits a GPU rectangle fill command.
 *
 *  Selects a packet format based on 64-pixel alignment of the horizontal fields.
 *  The color's high bit contributes to the GPU drawing mode.
 *  @param arg0 Four halfwords containing rectangle position and dimensions.
 *  @param arg1 Packed color and drawing-mode high bit.
 *  @return Always zero.
 *  @par Globals
 *  D_8009DDF4 and D_8009DDF6 bound the last two rectangle fields.
 *  D_8009DEC8 supplies drawing mode bits; D_800A6280 and D_800A62A8 hold packets.
 *  @par Control flow
 *  Clamps the last two fields, then chooses an aligned or unaligned packet.
 *  @par Calls
 *  func_800949E8 retrieves drawing settings for the unaligned packet.
 *  func_800949A0 submits the packet at D_800A6280.
 *  @par Side effects
 *  Modifies arg0, writes GPU packets, and submits a command.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 gSubmitGpuRectangleFill(u16 *arg0, u32 arg1)
{
    register u16 *rect __asm__("$8") = arg0;
    register u32 color __asm__("$9") = arg1;
    register s32 result __asm__("$2");
    rect[2] = (s16)rect[2] < 0 ? 0 : ((s16)gMaxDrawWidth - 1 < (s16)rect[2] ? gMaxDrawWidth - 1 : rect[2]);
    rect[3] = (s16)rect[3] < 0 ? 0 : ((s16)gMaxDrawHeight - 1 < (s16)rect[3] ? gMaxDrawHeight - 1 : rect[3]);

    if ((rect[0] & 0x3f) != 0 || (rect[2] & 0x3f) != 0) {
        register u32 mask __asm__("$6") = 0xffffff;
        register u32 limit __asm__("$4") = 0xe4ffffff;
        register u32 tag __asm__("$7") = 0x03ff0000;
        register u32 *p __asm__("$5");
        register u32 *q __asm__("$16");
        u32 xy, header;
        __asm__ volatile ("" : "+r"(mask), "+r"(limit), "+r"(tag));
        p = gGpuPacketArray;
        q = gGpuPacketArrayAlt;
        __asm__ volatile ("" : "+r"(p), "+r"(q));
        p[0] = ((u32)q & mask) | 0x08000000;
        p[1] = 0xe3000000;
        p[2] = limit;
        p[3] = 0xe5000000;
        p[4] = 0xe6000000;
        header = 0xe1000000 | ((color >> 31) << 10);
        p[5] = (*gGpuGp1PortPtr & 0x7ff) | header;
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
        p = gGpuPacketArray;
        __asm__ volatile ("" : "+r"(p));
        p[0] = value;
        p[1] = 0xe6000000;
        rgb = 0x02000000 | (color & mask);
        gpu = *gGpuGp1PortPtr;
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
    func_800949A0((u32)gGpuPacketArray);
    result = 0;
    __asm__ volatile ("" : "+r"(result));
    return result;
}
