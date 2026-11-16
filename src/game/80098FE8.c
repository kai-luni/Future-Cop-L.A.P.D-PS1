// Categories: Platform > SPU | Platform > Sound
#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    s16 left;
    s16 right;
} SpuVolume;

typedef struct {
    SpuVolume volume;
    s32 reverb;
    s32 mix;
} SpuExtAttr;

typedef struct {
    u32 mask;
    SpuVolume mvol;
    SpuVolume mvolmode;
    SpuVolume mvolx;
    SpuExtAttr cd;
    SpuExtAttr ext;
} SpuCommonAttr;

typedef struct {
    u8 pad00[0x180];
    u16 mainVolumeLeft;
    u16 mainVolumeRight;
    u8 pad184[0x26];
    u16 control;
    u8 pad1AC[4];
    u16 cdVolumeLeft;
    u16 cdVolumeRight;
    u16 extVolumeLeft;
    u16 extVolumeRight;
} SpuRegisters;

extern SpuRegisters *gSpuRegBasePtr __asm__("D_8009EEA4");

/**
 * @brief Writes common SPU volume and control attributes to hardware registers (SpuSetCommonAttr).
 *
 * Applies fields selected by attr->mask (or all fields when mask == 0) to D_8009EEA4 (0x1F801C00): configures mainVolumeLeft/Right (with optional mode encoding 0x8000..0xE000 and 0..0x7F clamping), cdVolumeLeft/Right, extVolumeLeft/Right, and CD/ext reverb/mix control bits 0..3 in D_8009EEA4->control.
 *
 * @param attr SpuCommonAttr struct holding bitmask, main/CD/ext volumes, and reverb/mix toggles.
 *
 * @par Globals
 * D_8009EEA4 - pointer to SPU MMIO register block (0x1F801C00).
 *
 * @par Control flow
 * Sequential mask checks: main left/right with 0..7 mode switch and 0..0x7F clamp, volume register writes, and control bit toggles.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes SPU MMIO registers mainVolumeLeft/Right, cdVolumeLeft/Right, extVolumeLeft/Right, and control.
 *
 * @par Callers
 * Confirmed: func_80021824 (0x80021870), func_80086C74 (0x80086CA8) / Likely: none / Unknown: none.
 */
void func_80098FE8(SpuCommonAttr *attr)
{
    s32 mask;
    s32 setAll;
    s32 leftVolume;
    s32 rightVolume;
    s32 mode;
    s16 signedVolume;
    s32 unsignedVolume;

    leftVolume = 0;
    __asm__("" : "=r"(leftVolume) : "0"(leftVolume));
    mask = attr->mask;
    setAll = mask == 0;
    rightVolume = 0;

    if (setAll || (mask & 1)) {
        if (setAll || (mask & 4)) {
            switch (attr->mvolmode.left) {
            case 1: mode = 0x8000; break;
            case 2: mode = 0x9000; break;
            case 3: mode = 0xA000; break;
            case 4: mode = 0xB000; break;
            case 5: mode = 0xC000; break;
            case 6: mode = 0xD000; break;
            case 7: mode = 0xE000; break;
            case 0:
                leftVolume = (u16)attr->mvol.left;
                mode = 0;
                break;
            default:
                leftVolume = (u16)attr->mvol.left;
                mode = 0;
                break;
            }
        } else {
            leftVolume = (u16)attr->mvol.left;
            mode = 0;
        }
        if (mode != 0) {
            unsignedVolume = (u16)attr->mvol.left;
            signedVolume = attr->mvol.left;
            if (signedVolume >= 0x80) {
                leftVolume = 0x7F;
            } else if (signedVolume >= 0) {
                leftVolume = unsignedVolume;
            } else {
                leftVolume = 0;
            }
        }
        gSpuRegBasePtr->mainVolumeLeft = (leftVolume & 0x7FFF) | mode;
    }

    if (setAll || (mask & 2)) {
        if (setAll || (mask & 8)) {
            switch (attr->mvolmode.right) {
            case 1: mode = 0x8000; break;
            case 2: mode = 0x9000; break;
            case 3: mode = 0xA000; break;
            case 4: mode = 0xB000; break;
            case 5: mode = 0xC000; break;
            case 6: mode = 0xD000; break;
            case 7: mode = 0xE000; break;
            case 0:
                rightVolume = (u16)attr->mvol.right;
                mode = 0;
                break;
            default:
                rightVolume = (u16)attr->mvol.right;
                mode = 0;
                break;
            }
        } else {
            rightVolume = (u16)attr->mvol.right;
            mode = 0;
        }
        if (mode != 0) {
            unsignedVolume = (u16)attr->mvol.right;
            signedVolume = attr->mvol.right;
            if (signedVolume >= 0x80) {
                rightVolume = 0x7F;
            } else if (signedVolume >= 0) {
                rightVolume = unsignedVolume;
            } else {
                rightVolume = 0;
            }
        }
        gSpuRegBasePtr->mainVolumeRight = (rightVolume & 0x7FFF) | mode;
    }

    if (setAll || (mask & 0x40)) {
        gSpuRegBasePtr->cdVolumeLeft = attr->cd.volume.left;
    }
    if (setAll || (mask & 0x80)) {
        gSpuRegBasePtr->cdVolumeRight = attr->cd.volume.right;
    }
    if (setAll || (mask & 0x400)) {
        gSpuRegBasePtr->extVolumeLeft = attr->ext.volume.left;
    }
    if (setAll || (mask & 0x800)) {
        gSpuRegBasePtr->extVolumeRight = attr->ext.volume.right;
    }

    if (setAll || (mask & 0x100)) {
        if (attr->cd.reverb == 0) {
            gSpuRegBasePtr->control &= ~4;
        } else {
            gSpuRegBasePtr->control |= 4;
        }
    }
    if (setAll || (mask & 0x200)) {
        if (attr->cd.mix == 0) {
            gSpuRegBasePtr->control &= ~1;
        } else {
            gSpuRegBasePtr->control |= 1;
        }
    }
    if (setAll || (mask & 0x1000)) {
        if (attr->ext.reverb == 0) {
            gSpuRegBasePtr->control &= ~8;
        } else {
            gSpuRegBasePtr->control |= 8;
        }
    }
    if (setAll || (mask & 0x2000)) {
        if (attr->ext.mix == 0) {
            gSpuRegBasePtr->control &= ~2;
        } else {
            gSpuRegBasePtr->control |= 2;
        }
    }
    __asm__ volatile("");
}
