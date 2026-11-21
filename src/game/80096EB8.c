// Categories: Platform > Sound | Platform > SPU
#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    u8 pad00[0x180];
    u16 f180;
    u16 f182;
    u16 f184;
    u16 f186;
    u16 f188;
    u16 f18A;
    u16 f18C;
    u16 f18E;
    u16 f190;
    u16 f192;
    u16 f194;
    u16 f196;
    u16 f198;
    u16 f19A;
    u16 f19C;
    u16 f19E;
    u16 f1A0;
    u16 f1A2;
    u16 f1A4;
    u16 f1A6;
    u16 f1A8;
    u16 f1AA;
    u16 f1AC;
    volatile u16 f1AE;
    u16 f1B0;
    u16 f1B2;
    u16 f1B4;
    u16 f1B6;
} Spu;

typedef struct {
    u16 f0;
    u16 f2;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fA;
    u16 fC;
    u16 fE;
} Voice;

extern u32 *gSpuDelayCtrlPtr __asm__("D_8009EEB4");
extern Spu *gSpuRegBasePtr __asm__("D_8009EEA4");
extern s32 gOpOrderFlag __asm__("D_8009EEC0");
extern s32 D_8009EEC4;
extern s32 gSpuAlignEnable __asm__("D_8009EEC8");
extern s32 gSpuAddrShift __asm__("D_8009EECC");
extern s32 gSpuAlignBase __asm__("D_8009EED0");
extern s32 gSpuAlignMask __asm__("D_8009EED4");
extern s32 gSpuClearFlag __asm__("D_8009EED8");
extern s32 gSpuTransferCallback __asm__("D_8009EEDC");
extern s32 gInstalledHandler __asm__("D_8009EEE0");
extern u8 gSpuInitData[] __asm__("D_8009EEE4");
extern u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern u16 gSpuInitWord[] __asm__("D_800A7028");
extern u8 gSpuTimeoutFormat[] __asm__("D_80012CDC");
extern u8 D_80012CEC[];

extern void gSpuBusyWait(void) __asm__("func_80097990");
extern void func_8008E000(void *fmt, void *msg);
extern void gSpuWriteMemBlock(void *attr, s32 size) __asm__("func_80097138");

/**
 * @brief Resets SPU hardware registers and optionally initializes all 24 SPU voices.
 *
 * Sets *D_8009EEB4 |= 0xB0000, mutes main and SPU control registers, polls spu->f1AE & 0x7FF (logging timeout after 0xF00 loops), configures reverb/timing globals and D_800A7028, and on warm start (arg0 == 0) uploads 0x10 bytes of D_8009EEE4 to SPU RAM 0x200, initializes 24 voices, keys them off, and sets control 0xC000.
 *
 * @param arg0 0 performs full voice/RAM reset; non-zero skips voice table and SPU RAM setup.
 * @return Always 0.
 *
 * @par Globals
 * D_8009EEA4 - SPU base (0x1F801C00); D_8009EEB4 - SPU delay/control ptr; D_800A7028/D_8009EEE4/D_8009EEC0..D_8009EEE0/D_8009EEBC - SPU driver state;
 * D_8009EEDC (gSpuTransferCallback) is the transfer-complete callback and is
 * cleared here.
 *
 * @par Control flow
 * Busy-wait loop with 0xF00 timeout cap, 10-entry clear loop, conditional warm-start block with 24-voice init loop, and final enable.
 *
 * @par Calls
 * gSpuBusyWait - hardware delay loop; func_8008E000 - prints "SPU: T/O [wait (reset)]"; gSpuWriteMemBlock - uploads D_8009EEE4 to SPU RAM.
 *
 * @par Side effects
 * Writes SPU MMIO registers (0x1F801C00..), SPU RAM at 0x200, D_800A7028, and SPU globals.
 *
 * @par Callers
 * Confirmed: gSpuInitSubsystem (0x80096D6C, SPU subsystem init) / Likely: none / Unknown: none.
 */
s32 gSpuResetHardware(s32 arg0) __asm__("func_80096EB8");

s32 gSpuResetHardware(s32 arg0)
{
    register Spu *spu __asm__("$2");
    register Voice *voice __asm__("$3");
    register u32 k __asm__("$3");
    register u16 *p __asm__("$5");
    s32 i;
    u32 c1;
    u32 c2;
    u32 v;
    register Spu *tl __asm__("$4");
    u8 *arr;
    register s32 ret __asm__("$2");

    v = *gSpuDelayCtrlPtr | 0xB0000;
    *gSpuDelayCtrlPtr = v;
    spu = gSpuRegBasePtr;
    gOpOrderFlag = 0;
    D_8009EEC4 = 0;
    gSpuTransferAddr = 0;
    spu->f180 = 0;
    spu->f182 = 0;
    spu->f1AA = 0;
    gSpuBusyWait();
    spu = gSpuRegBasePtr;
    spu->f180 = 0;
    spu->f182 = 0;
    __asm__ volatile("" ::: "memory");
    k = 0;
    if ((spu->f1AE & 0x7FF) == 0) {
        goto cont;
    }
again:
    k++;
    if (k >= 0xF01) {
        __asm__ volatile("" ::: "memory");
        func_8008E000(gSpuTimeoutFormat, D_80012CEC);
        goto cont;
    }
    spu = gSpuRegBasePtr;
    if ((spu->f1AE & 0x7FF) != 0) {
        goto again;
    }
cont:
    i = 0;
    p = gSpuInitWord;
    gSpuAlignEnable = 2;
    gSpuAddrShift = 3;
    gSpuAlignBase = 8;
    gSpuAlignMask = 7;
    __asm__ volatile("" ::: "memory");
    spu = gSpuRegBasePtr;
    spu->f1AC = 4;
    spu->f184 = 0;
    spu->f186 = 0;
    spu->f18C = 0xFFFF;
    spu->f18E = 0xFFFF;
    spu->f198 = 0;
    spu->f19A = 0;
    do {
        *p = 0;
        i++;
        __asm__("" : "+r"(i));
        p++;
    } while (i < 10);
    if (arg0 != 0) {
        goto done;
    }
    arr = gSpuInitData;
    spu = gSpuRegBasePtr;
    __asm__ volatile("" : : "r"(spu));
    gSpuTransferAddr = 0x200;
    spu->f190 = 0;
    spu->f192 = 0;
    spu->f194 = 0;
    spu->f196 = 0;
    spu->f1B0 = 0;
    spu->f1B2 = 0;
    spu->f1B4 = 0;
    spu->f1B6 = 0;
    __asm__ volatile("" ::: "memory");
    gSpuWriteMemBlock(arr, 0x10);
    i = 0;
    c1 = 0x3FFF;
    c2 = 0x200;
    voice = (Voice *)gSpuRegBasePtr;
    __asm__ volatile("nop" : : "r"(voice), "r"(c1), "r"(c2));
    do {
        voice->f0 = 0;
        voice->f2 = 0;
        voice->f4 = c1;
        voice->f6 = c2;
        voice->f8 = 0;
        voice->fA = 0;
        __asm__ volatile("" : "+r"(i) : : "memory");
        i++;
        voice++;
    } while (i < 0x18);
    spu = gSpuRegBasePtr;
    spu->f188 = 0xFFFF;
    spu->f18A = 0xFF;
    gSpuBusyWait();
    gSpuBusyWait();
    gSpuBusyWait();
    gSpuBusyWait();
    spu = gSpuRegBasePtr;
    spu->f18C = 0xFFFF;
    spu->f18E = 0xFF;
    gSpuBusyWait();
    gSpuBusyWait();
    gSpuBusyWait();
    gSpuBusyWait();
done:
    ret = 0;
    __asm__ volatile("" : "+r"(ret));
    tl = gSpuRegBasePtr;
    gSpuClearFlag = 1;
    tl->f1AA = 0xC000;
    __asm__ volatile("" ::: "memory");
    gSpuTransferCallback = 0;
    gInstalledHandler = 0;
    __asm__ volatile("" ::: "memory");
    return ret;
}
