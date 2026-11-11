#include "game/types.h"

extern s32 D_8009F330;
extern s32 D_8009F730;
extern s32 D_8009F734;
extern s32 D_800A1920[];
extern s32 func_8001A074(s32 *);

#define CLAMP(x, lo, hi) ((x) >= (lo) ? ((x) <= (hi) ? (x) : (hi)) : (lo))
#define RAW(d) ((d) < 0 ? D_8009F330 : (d) <= 0x20000 ? D_8009F330 - (((d) * D_8009F330) >> 17) : D_8009F330 - ((0x20000 * D_8009F330) >> 17))
#define ATTENUATE(d) CLAMP(RAW(d), 0, D_8009F330)
#define OVER_FAST(d) ((d) < 0 ? 0 : ({ s32 limit = D_8009F330; ((d) <= 0x20000 ? limit < limit - (((d) * limit) >> 17) : limit < limit - ((0x20000 * limit) >> 17)); }))
#define ATTEN_FAST(d) (RAW(d) < 0 ? 0 : OVER_FAST(d) ? D_8009F330 : RAW(d))
#define OVER_SC(d, lim) ((d) >= 0 && ({ lim = D_8009F330; 1; }) && ((d) <= 0x20000 ? lim < lim - (((d) * lim) >> 17) : lim < lim - ((0x20000 * lim) >> 17)))
#define ATTEN_SC(d, lim) (RAW(d) < 0 ? 0 : OVER_SC(d, lim) ? D_8009F330 : RAW(d))
#define ATTEN_SC_REG(d, r) ({ register s32 out __asm__(r); out = (RAW(d) < 0 ? 0 : ({ s32 limit; (OVER_SC(d, limit) ? D_8009F330 : RAW(d)); })); out; })
#define OVER(d) ((d) < 0 ? 0 : D_8009F330 < D_8009F330 - (((d) <= 0x20000 ? (d) * D_8009F330 : 0x20000 * D_8009F330) >> 17))
#define ATTEN_MID(d) (RAW(d) < 0 ? 0 : OVER(d) ? D_8009F330 : RAW(d))

struct Mix458 {
    s8 volume, pan, flag, pad;
    s32 dx0, dz0, dx1, dz1;
};
struct Extra458 {
    u8 pad[0xAC];
    struct ExtraData458 *data;
};
struct ExtraData458 {
    u8 pad[0xC1];
    s8 mode;
};

s32 func_800458E8(s32 xin, s32 zin, struct Mix458 *mixin, struct Extra458 *extra)
{
    register s32 x __asm__("$19") = xin;
    register s32 z __asm__("$20") = zin;
    register struct Mix458 *mix __asm__("$18") = mixin;
    register s32 *base __asm__("$16");
    s32 d0, d1, amount, dx, dz, value;
    register s32 bound __asm__("$2");
    register s32 scaled __asm__("$5");
    s32 result = 1;
    __asm__("" : "+r"(x), "+r"(z), "+r"(mix));
    if (x == 0 && z == 0) {
        mix->volume = D_8009F330;
        if (D_8009F734 == 2 && D_8009F730 != 0 && extra != 0) {
            switch (extra->data->mode) {
            case 0:
                mix->pan = 0;
                break;
            case 1:
                mix->pan = 127;
                break;
            default:
                mix->pan = 64;
                break;
            }
        } else {
            mix->pan = 64;
        }
        mix->flag = 1;
    } else switch (D_8009F734) {
    default:
        mix->volume = D_8009F330;
        mix->pan = 64;
        mix->flag = 1;
        break;
    case 1:
        base = D_800A1920;
        __asm__("" : "+r"(base));
        dx = x - D_800A1920[0];
        dz = z - base[2];
        __asm__("" : "+r"(dx), "+r"(dz));
        mix->dx0 = dx;
        mix->dz0 = dz;
        d0 = func_8001A074(&mix->dx0);
        if (d0 > 0x1FFFF) {
            result = 0;
            mix->volume = 0;
            mix->pan = 64;
            mix->flag = 1;
        } else {
            bound = D_8009F330;
            __asm__("" : "+r"(bound));
            scaled = (d0 * bound) >> 17;
            amount = bound - scaled;
            if (amount >= 0) {
                value = *(u8 *)&D_8009F330;
                if (!(bound < amount)) value -= scaled;
            } else value = 0;
            mix->volume = value;
            if (D_8009F730 != 0) {
                mix->flag = 0;
            } else {
                mix->pan = 64;
                mix->flag = 1;
            }
        }
        break;
    case 2:
        {
            register s32 *arg0 __asm__("$4");
            base = D_800A1920;
            __asm__("" : "+r"(base));
            arg0 = &mix->dx0;
            __asm__("" : "+r"(arg0));
            dx = x - D_800A1920[0];
            dz = z - base[2];
            base += 4;
            __asm__("" : "+r"(dx), "+r"(dz), "+r"(base));
            mix->dx0 = dx;
            mix->dz0 = dz;
            d0 = func_8001A074(arg0);
        }
        dx = x - base[0];
        dz = z - base[2];
        __asm__("" : "+r"(dx), "+r"(dz));
        mix->dx1 = dx;
        mix->dz1 = dz;
        d1 = func_8001A074(&mix->dx1);
        if (d0 > 0x1FFFF && d1 > 0x1FFFF) {
            result = 0;
            mix->volume = 0;
            mix->pan = 64;
            mix->flag = 1;
        } else {
            s32 lim3, lim5;
            mix->volume = ATTEN_SC_REG(d1, "$4") - (({ register s32 b2 __asm__("$5"); b2 = (ATTEN_SC_REG(d1, "$5") - ATTEN_SC(d0, lim3)) >> 31; b2; }) & (ATTEN_SC_REG(d1, "$6") - ATTEN_SC(d0, lim5)));
            if (D_8009F730 != 0) {
                mix->flag = 0;
            } else {
                mix->pan = 64;
                mix->flag = 1;
            }
        }
        break;
    }
    return result;
}
