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

extern u32 *D_8009EEB4;
extern Spu *D_8009EEA4;
extern s32 D_8009EEC0;
extern s32 D_8009EEC4;
extern s32 D_8009EEC8;
extern s32 D_8009EECC;
extern s32 D_8009EED0;
extern s32 D_8009EED4;
extern s32 D_8009EED8;
extern s32 D_8009EEDC;
extern s32 D_8009EEE0;
extern u8 D_8009EEE4[];
extern u16 D_8009EEBC;
extern u16 D_800A7028[];
extern u8 D_80012CDC[];
extern u8 D_80012CEC[];

extern void func_80097990(void);
extern void func_8008E000(void *fmt, void *msg);
extern void func_80097138(void *attr, s32 size);

s32 func_80096EB8(s32 arg0)
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

    v = *D_8009EEB4 | 0xB0000;
    *D_8009EEB4 = v;
    spu = D_8009EEA4;
    D_8009EEC0 = 0;
    D_8009EEC4 = 0;
    D_8009EEBC = 0;
    spu->f180 = 0;
    spu->f182 = 0;
    spu->f1AA = 0;
    func_80097990();
    spu = D_8009EEA4;
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
        func_8008E000(D_80012CDC, D_80012CEC);
        goto cont;
    }
    spu = D_8009EEA4;
    if ((spu->f1AE & 0x7FF) != 0) {
        goto again;
    }
cont:
    i = 0;
    p = D_800A7028;
    D_8009EEC8 = 2;
    D_8009EECC = 3;
    D_8009EED0 = 8;
    D_8009EED4 = 7;
    __asm__ volatile("" ::: "memory");
    spu = D_8009EEA4;
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
    arr = D_8009EEE4;
    spu = D_8009EEA4;
    __asm__ volatile("" : : "r"(spu));
    D_8009EEBC = 0x200;
    spu->f190 = 0;
    spu->f192 = 0;
    spu->f194 = 0;
    spu->f196 = 0;
    spu->f1B0 = 0;
    spu->f1B2 = 0;
    spu->f1B4 = 0;
    spu->f1B6 = 0;
    __asm__ volatile("" ::: "memory");
    func_80097138(arr, 0x10);
    i = 0;
    c1 = 0x3FFF;
    c2 = 0x200;
    voice = (Voice *)D_8009EEA4;
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
    spu = D_8009EEA4;
    spu->f188 = 0xFFFF;
    spu->f18A = 0xFF;
    func_80097990();
    func_80097990();
    func_80097990();
    func_80097990();
    spu = D_8009EEA4;
    spu->f18C = 0xFFFF;
    spu->f18E = 0xFF;
    func_80097990();
    func_80097990();
    func_80097990();
    func_80097990();
done:
    ret = 0;
    __asm__ volatile("" : "+r"(ret));
    tl = D_8009EEA4;
    D_8009EED8 = 1;
    tl->f1AA = 0xC000;
    __asm__ volatile("" ::: "memory");
    D_8009EEDC = 0;
    D_8009EEE0 = 0;
    __asm__ volatile("" ::: "memory");
    return ret;
}
