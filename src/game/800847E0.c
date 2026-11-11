typedef unsigned char u8;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

extern u8 *D_8009F9D8;
extern u8 *D_8009FABC;
extern s32 D_8009F4BC;
extern s32 D_8009F4C0;
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;

#define K(o, e, tp, i) o = (e); if ((o & 7) >= (tp)[i]) o += 8; o = D_8009F9D8[o];
#define PACK (a | ((b << 5) | (c << 10)))

void func_800847E0(u32 *src, u32 *dst, s16 *arg)
{
    u8 *th;
    u8 *end;
    int n;
    u32 w;
    u32 a, b, c, p0, pc;

    th = D_8009FABC;
    n = arg[3];
    end = th + 0x100;
    if (n > 0) {
        do {
            w = src[0];
            K(c, ((w >> 16) & 0xFF), th, 0);
            K(b, ((w >> 8) & 0xFF), th, 0);
            K(a, (w & 0xFF), th, 0);
            pc = (c << 10) | (b << 5);
            p0 = pc | a;
            K(a, ((w >> 24) & 0xFF), th, 1);
            w = src[1];
            K(c, ((w >> 8) & 0xFF), th, 1);
            K(b, (w & 0xFF), th, 1);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[0] = p0;
            K(b, ((w >> 24) & 0xFF), th, 2);
            K(a, ((w >> 16) & 0xFF), th, 2);
            w = src[2];
            K(c, (w & 0xFF), th, 2);
            p0 = (c << 10) | (b << 5) | a;
            K(c, ((w >> 24) & 0xFF), th, 3);
            K(b, ((w >> 16) & 0xFF), th, 3);
            K(a, ((w >> 8) & 0xFF), th, 3);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[1] = p0;
            w = src[3];
            K(c, ((w >> 16) & 0xFF), th, 4);
            K(b, ((w >> 8) & 0xFF), th, 4);
            K(a, (w & 0xFF), th, 4);
            p0 = (c << 10) | (b << 5) | a;
            K(a, ((w >> 24) & 0xFF), th, 5);
            w = src[4];
            K(c, ((w >> 8) & 0xFF), th, 5);
            K(b, (w & 0xFF), th, 5);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[2] = p0;
            K(b, ((w >> 24) & 0xFF), th, 6);
            K(a, ((w >> 16) & 0xFF), th, 6);
            w = src[5];
            K(c, (w & 0xFF), th, 6);
            p0 = (c << 10) | (b << 5) | a;
            K(c, ((w >> 24) & 0xFF), th, 7);
            K(b, ((w >> 16) & 0xFF), th, 7);
            K(a, ((w >> 8) & 0xFF), th, 7);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[3] = p0;
            w = src[6];
            K(c, ((w >> 16) & 0xFF), th, 8);
            K(b, ((w >> 8) & 0xFF), th, 8);
            K(a, (w & 0xFF), th, 8);
            p0 = (c << 10) | (b << 5) | a;
            K(a, ((w >> 24) & 0xFF), th, 9);
            w = src[7];
            K(c, ((w >> 8) & 0xFF), th, 9);
            K(b, (w & 0xFF), th, 9);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[4] = p0;
            K(b, ((w >> 24) & 0xFF), th, 10);
            K(a, ((w >> 16) & 0xFF), th, 10);
            w = src[8];
            K(c, (w & 0xFF), th, 10);
            p0 = (c << 10) | (b << 5) | a;
            K(c, ((w >> 24) & 0xFF), th, 11);
            K(b, ((w >> 16) & 0xFF), th, 11);
            K(a, ((w >> 8) & 0xFF), th, 11);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[5] = p0;
            w = src[9];
            K(c, ((w >> 16) & 0xFF), th, 12);
            K(b, ((w >> 8) & 0xFF), th, 12);
            K(a, (w & 0xFF), th, 12);
            p0 = (c << 10) | (b << 5) | a;
            K(a, ((w >> 24) & 0xFF), th, 13);
            w = src[10];
            K(c, ((w >> 8) & 0xFF), th, 13);
            K(b, (w & 0xFF), th, 13);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[6] = p0;
            K(b, ((w >> 24) & 0xFF), th, 14);
            K(a, ((w >> 16) & 0xFF), th, 14);
            w = src[11];
            K(c, (w & 0xFF), th, 14);
            p0 = (c << 10) | (b << 5) | a;
            K(c, ((w >> 24) & 0xFF), th, 15);
            K(b, ((w >> 16) & 0xFF), th, 15);
            K(a, ((w >> 8) & 0xFF), th, 15);
            p0 |= (((c << 10) | (b << 5)) | a) << 16;
            dst[7] = p0;
            dst += 8;
            src += 12;
            th += 16;
            if (th < end) {
            } else {
                th = D_8009FABC;
            }
        } while (--n > 0);
    }
}
