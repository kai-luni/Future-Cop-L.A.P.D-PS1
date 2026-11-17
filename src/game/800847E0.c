// Categories: Graphics > Frame Conversion | Graphics > Video Decode Output
typedef unsigned char u8;
typedef unsigned int u32;
typedef short s16;
typedef int s32;

extern u8 *D_8009F9D8;
extern u8 *D_8009FABC;
extern s32 gDecodeJob __asm__("D_8009F4BC");
extern s32 D_8009F4C0;
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;

#define K(o, e, tp, i) o = (e); if ((o & 7) >= (tp)[i]) o += 8; o = D_8009F9D8[o];
#define PACK (a | ((b << 5) | (c << 10)))

/**
 * @brief Converts one row of 24-bit RGB pixels into 15-bit RGB555 with a
 *        dither-controlled rounding step and writes two pixels per word.
 *
 * @param src Source row, read as src[0] .. src[11] (+0x00 .. +0x2C): 48 bytes
 *            per row, i.e. 16 three-byte pixels.
 * @param dst Destination row, written as dst[0] .. dst[7] (+0x00 .. +0x1C):
 *            8 words, each holding two packed 15-bit pixels.
 * @param arg Rectangle descriptor; only arg[3] (byte offset +0x06) is used and
 *            it is the number of rows to convert. The caller passes a
 *            {u16 x, y, w, h} rectangle, so +0x06 is its h field.
 * @return Nothing (the function is declared void).
 *
 * @par Confirmed
 * One iteration converts one row and advances src by 12 words, dst by 8 words
 * and the threshold pointer by 16 bytes; the loop runs arg[3] times and is
 * skipped when arg[3] <= 0. Per row exactly 48 channel values are produced
 * (3 per output pixel times 16 pixels) and 8 destination words are written, so
 * the input and the output have the same pixel count (16) and the source is a
 * 3-byte-per-pixel format.
 * The K macro resolves one channel: the byte is kept, +8 is added when its low
 * three bits are >= the threshold byte selected for that output pixel, and the
 * result indexes the byte table D_8009F9D8. Three resolved bytes are packed as
 * (c << 10) | (b << 5) | a; the first three go into the low half of the
 * destination word and the next three into the high half, so one word carries
 * two pixels and consumes 6 channel values.
 * The 16 output pixels of a row use thresholds 0..15 of a 16-byte block taken
 * from the 0x100-byte table at D_8009FABC. Because the pointer advances by 16
 * after every row and is reset to the table base once it reaches th + 0x100,
 * the table is 16 pattern rows of 16 bytes and the rows cycle.
 * The +8 bump makes indices up to 15 + 8 = 23 reachable per group, which is why
 * the lookup table is longer than 256 bytes: the movie path copies a 0x100-byte
 * and a 0x108-byte table into the scratchpad (see func_80085228), and 0x108 is
 * the 264-entry size that bump needs.
 *
 * @par Likely role
 * The 24-bit-to-15-bit row converter of the movie playback path. The
 * frame-complete callback func_80085074 scales each decoder strip of a 320-wide
 * frame to 2/3 width into a local rectangle and calls this with src == dst, so
 * the conversion runs in place. The threshold table is an ordered dither that
 * breaks up the 5-bit banding of the truncation, and D_8009F9D8 is read as the
 * "rounded byte to 5-bit value" map that finishes the quantisation.
 *
 * @par Unknown
 * Whether the three bytes of a source pixel are R, G, B or B, G, R: the code
 * puts the lowest-addressed byte into bits 0..4, which matches the PS1 15-bit
 * layout only for one of the two orders. The threshold pattern values in
 * D_8009FABC are data and not part of this translation unit.
 *
 * @par Callers
 * func_80085074 only, the frame-complete callback of the video decoder.
 */
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
