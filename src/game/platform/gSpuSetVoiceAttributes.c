// Categories: Platform > Audio
#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    s16 left;
    s16 right;
} SpuVolume;

typedef struct {
    u32 voice;
    u32 mask;
    SpuVolume volume;
    SpuVolume volmode;
    SpuVolume volumex;
    u16 pitch;
    u16 note;
    u16 sample_note;
    s16 envx;
    u32 addr;
    u32 loop_addr;
    s32 a_mode;
    s32 s_mode;
    s32 r_mode;
    u16 ar;
    u16 dr;
    u16 sr;
    u16 rr;
    u16 sl;
    u16 adsr1;
    u16 adsr2;
} SpuVoiceAttr;

typedef struct {
    SpuVolume volume;
    u16 pitch;
    u16 addr;
    u16 adsr1;
    u16 adsr2;
    u16 volumex;
    u16 loop_addr;
} SpuVoiceReg;

typedef union {
    u16 raw[0x100];
    SpuVoiceReg voice[24];
} SpuRXX;

extern SpuRXX *gSpuRegBasePtr __asm__("D_8009EEA4");
extern u16 gVoiceSampleNote[] __asm__("D_8009EA60");
extern u16 gConvertSpuNoteToPitch(s32, s32, s32, s32) __asm__("func_800999F4");
extern void gSpuEncodeOrWriteAddr(s32, u32) __asm__("func_80097808");

/**
 *  @brief Applies selected SPU voice attributes to the specified voices.
 *
 *  A zero attribute mask selects all supported fields; otherwise mask bits select
 *  pitch, volume, addresses, and envelope fields for each voice in the voice bitset.
 *  @param arg Voice selection, attribute mask, and values to apply.
 *  @par Globals
 *  D_8009EEA4 points to the SPU register layout written by this function.
 *  D_8009EA60 stores the sample note associated with each voice.
 *  @par Control flow
 *  Iterates over 24 voices, skipping unselected ones and branching on attribute bits.
 *  Volume modes are encoded and sweep volumes and envelope rates are clamped.
 *  Raw ADSR values are written before any selected individual envelope fields.
 *  @par Calls
 *  func_800999F4 converts sample and target notes to pitch.
 *  func_80097808 writes the selected start or loop address.
 *  @par Side effects
 *  Updates SPU voice registers and the per-voice sample-note table.
 *  @par Callers
 *  Confirmed: func_80021B90, func_80021BD0, func_80086C74.
 */
void gSpuSetVoiceAttributes(SpuVoiceAttr *arg) __asm__("func_80099368");

void gSpuSetVoiceAttributes(SpuVoiceAttr *arg)
{
    s32 ch;
    s32 ch8;
    u16 *note;
    s32 mask;
    s32 set_all;
    s32 vol_mode;
    s32 volume;
    u32 rate;
    s32 mode;
    register u32 old __asm__("$2");
    register u32 masked __asm__("$3");
    volatile s32 i;
    volatile s32 j;

    ch = 0;
    mask = arg->mask;
    note = gVoiceSampleNote;
    set_all = (mask == 0);

    for (; ch < 24; ch++, note++) {
        if ((arg->voice & (1 << ch)) == 0) {
            continue;
        }
        ch8 = ch << 3;
        if (set_all || (mask & 0x10)) {
            (gSpuRegBasePtr->voice + ch)->pitch = arg->pitch;
        }
        if (set_all || (mask & 0x40)) {
            *note = arg->sample_note;
        }
        if (set_all || (mask & 0x20)) {
            gSpuRegBasePtr->raw[ch8 + 2] = gConvertSpuNoteToPitch(*note >> 8, *note & 0xFF,
                                                     arg->note >> 8, arg->note & 0xFF);
        }
        if (set_all || (mask & 0x1)) {
            vol_mode = 0;
            volume = arg->volume.left & 0x7FFF;
            if (set_all || (mask & 0x4)) {
                switch (arg->volmode.left) {
                case 1: vol_mode = 0x8000; break;
                case 2: vol_mode = 0x9000; break;
                case 3: vol_mode = 0xA000; break;
                case 4: vol_mode = 0xB000; break;
                case 5: vol_mode = 0xC000; break;
                case 6: vol_mode = 0xD000; break;
                case 7: vol_mode = 0xE000; break;
                }
            }
            if (vol_mode != 0) {
                if (arg->volume.left >= 0x80) {
                    volume = 0x7F;
                } else if (arg->volume.left < 0) {
                    volume = 0;
                }
            }
            gSpuRegBasePtr->raw[ch8] = volume | vol_mode;
        }
        if (set_all || (mask & 0x2)) {
            vol_mode = 0;
            volume = arg->volume.right & 0x7FFF;
            if (set_all || (mask & 0x8)) {
                switch (arg->volmode.right) {
                case 1: vol_mode = 0x8000; break;
                case 2: vol_mode = 0x9000; break;
                case 3: vol_mode = 0xA000; break;
                case 4: vol_mode = 0xB000; break;
                case 5: vol_mode = 0xC000; break;
                case 6: vol_mode = 0xD000; break;
                case 7: vol_mode = 0xE000; break;
                }
            }
            if (vol_mode != 0) {
                if (arg->volume.right >= 0x80) {
                    volume = 0x7F;
                } else if (arg->volume.right < 0) {
                    volume = 0;
                }
            }
            gSpuRegBasePtr->raw[ch8 + 1] = volume | vol_mode;
        }
        if (set_all || (mask & 0x80)) {
            gSpuEncodeOrWriteAddr(ch8 | 3, arg->addr);
        }
        if (set_all || (mask & 0x10000)) {
            gSpuEncodeOrWriteAddr(ch8 | 7, arg->loop_addr);
        }
        if (set_all || (mask & 0x20000)) {
            gSpuRegBasePtr->raw[ch8 + 4] = arg->adsr1;
        }
        if (set_all || (mask & 0x40000)) {
            gSpuRegBasePtr->raw[ch8 + 5] = arg->adsr2;
        }
        if (set_all || (mask & 0x800)) {
            rate = arg->ar;
            if (rate >= 0x80) {
                rate = 0x7F;
            }
            mode = 0;
            if (set_all || (mask & 0x100)) {
                if (arg->a_mode == 5) {
                    mode = 0x80;
                }
            }
            old = gSpuRegBasePtr->raw[ch8 + 4];
            __asm__ volatile("" : "=r"(old) : "0"(old));
            masked = old & 0xFF;
            __asm__ volatile("" : "=r"(masked) : "0"(masked));
            gSpuRegBasePtr->raw[ch8 + 4] = masked | ((rate | mode) << 8);
        }
        if (set_all || (mask & 0x1000)) {
            rate = arg->dr;
            if (rate >= 0x10) {
                rate = 0xF;
            }
            gSpuRegBasePtr->raw[ch8 + 4] = (gSpuRegBasePtr->raw[ch8 + 4] & 0xFF0F) | (rate << 4);
        }
        if (set_all || (mask & 0x2000)) {
            rate = arg->sr;
            if (rate >= 0x80) {
                rate = 0x7F;
            }
            mode = 0x100;
            if (set_all || (mask & 0x200)) {
                switch (arg->s_mode) {
                case 1: mode = 0; break;
                case 5: mode = 0x200; break;
                case 7: mode = 0x300; break;
                }
            }
            old = gSpuRegBasePtr->raw[ch8 + 5];
            __asm__ volatile("" : "=r"(old) : "0"(old));
            masked = old & 0x3F;
            __asm__ volatile("" : "=r"(masked) : "0"(masked));
            gSpuRegBasePtr->raw[ch8 + 5] = masked | ((rate | mode) << 6);
        }
        if (set_all || (mask & 0x4000)) {
            rate = arg->rr;
            if (rate >= 0x20) {
                rate = 0x1F;
            }
            mode = 0;
            if (set_all || (mask & 0x400)) {
                switch (arg->r_mode) {
                case 3: mode = 0; break;
                case 7: mode = 0x20; break;
                }
            }
            gSpuRegBasePtr->raw[ch8 + 5] = (gSpuRegBasePtr->raw[ch8 + 5] & 0xFFC0) | (rate | mode);
        }
        if (set_all || (mask & 0x8000)) {
            rate = arg->sl;
            if (rate >= 0x10) {
                rate = 0xF;
            }
            gSpuRegBasePtr->raw[ch8 + 4] = (gSpuRegBasePtr->raw[ch8 + 4] & 0xFFF0) | rate;
        }
    }
    j = 1;
    for (i = 0; i < 2; i = i + 1) {
        j *= 13;
    }
}
