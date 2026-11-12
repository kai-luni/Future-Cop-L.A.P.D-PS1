typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;

extern u16 D_8009DDF4, D_8009DDF6;
extern volatile u32 *D_8009DEC8, *D_8009DECC, *D_8009DED0, *D_8009DED4;
extern u32 *volatile D_8009DEC4;
extern void func_800951D8(void);
extern int func_8009520C(void);

int func_8009446C(u16 *hdr_, u32 *data_)
{
    u32 *data = data_;
    u16 *hdr = hdr_;
    int dir = 0;
    int rem, tmp;
    int blocks;
    u16 a; int n, words, t;
    s16 sv; u16 uv; s16 sv2; u16 uv2; u16 b;
    u32 first;

    func_800951D8();
    sv = hdr[2]; uv = hdr[2];
    if (sv >= 0) {
        a = uv;
        if ((s16)D_8009DDF4 < sv) a = D_8009DDF4;
    } else {
        a = 0;
    }
    hdr[2] = a;
    sv2 = hdr[3]; uv2 = hdr[3];
    if (sv2 >= 0) {
        b = uv2;
        if ((s16)D_8009DDF6 < sv2) b = D_8009DDF6;
    } else {
        b = 0;
    }
    hdr[3] = b;
    n = (s16)hdr[2] * (s16)hdr[3];
    t = n + 1;
    words = t / 2;
    if (words <= 0) return -1;
    rem = words >> 4; tmp = rem; rem = words - tmp * 16; blocks = tmp;
    if ((*D_8009DEC8 & 0x4000000) == 0) {
        __asm__ volatile("" ::: "$3");
        do {
            if (func_8009520C() != 0) return -1;
        } while ((*D_8009DEC8 & 0x4000000) == 0);
    }
    *D_8009DEC8 = 0x4000000;
    *(volatile u32 *)D_8009DEC4 = 0x1000000;
    *(volatile u32 *)D_8009DEC4 = dir ? 0xB0000000 : 0xA0000000;
    *(volatile u32 *)D_8009DEC4 = *(u32 *)hdr;
    *(volatile u32 *)D_8009DEC4 = ((u32 *)hdr)[1];
    rem--;
    if (rem != -1) {
        __asm__ volatile("" ::: "$2");
        do {
            *D_8009DEC4 = *data++;
        } while (--rem != -1);
    }
    if (blocks != 0) {
        *D_8009DEC8 = 0x4000002;
        *D_8009DECC = (u32)data;
        *D_8009DED0 = (blocks << 16) | 0x10;
        *D_8009DED4 = 0x1000201;
    }
    return 0;
}
