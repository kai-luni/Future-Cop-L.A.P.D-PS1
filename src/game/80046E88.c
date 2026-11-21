// Categories: Platform > Sound > Voices
#include "game/types.h"

struct SoundBank {
    s32 samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    s32 head;
};

extern u8 gSoundBankTableLow[] __asm__("D_800A1830");
extern struct SoundBank gSoundEntryTable[] __asm__("D_800A7938");

extern s32 gResolveSoundRecordIndex(s32 id, u8 *table, s32 limit) __asm__("func_80045494");
extern s32 gComputeSpatialSoundAttenuation(s32 arg0, s32 arg1, s8 *mix, void *arg3) __asm__("func_800458E8");
extern s32 gAllocPrioritySoundVoice(s32 volume, s32 arg1, s32 arg2, u32 index) __asm__("func_80046B04");
extern void gComputeSpatialSoundPan(s8 *mix) __asm__("func_800460D4");
extern u8 gPlayNextBankSample(struct SoundBank *bank, s32 handle, s32 pan, s32 volume, s32 extra) __asm__("func_8004772C");
extern void gReleaseSoundVoice(s32 handle) __asm__("func_80046698");
s32 gPlaySoundIdOnVoice(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object) __asm__("func_80046E88");


/**
 * @brief Resolves a sound track ID, computes spatial mix parameters, and starts voice playback.
 *
 * Looks up track in D_800A1830/D_800A18D0 via func_80045494, computes volume and spatial pan from (arg1, arg2, object), allocates a voice when handle is 0, and submits the sample through func_8004772C.
 *
 * @param track Sound/track ID (bound 0x47 against D_800A1830).
 * @param arg1 World X position (or 0).
 * @param arg2 World Z position (or 0).
 * @param arg3 Extra voice parameter.
 * @param handle Existing voice handle, or 0 to allocate one.
 * @param object Optional entity pointer passed to mix setup.
 * @return Newly allocated voice handle if handle was 0 and start succeeded, or 0 otherwise (including when an existing handle was supplied).
 * @par Globals
 * D_800A1830 - primary sound ID mapping table; D_800A7938 - sound bank descriptor array.
 * @par Control flow
 * Returns 0 if func_80045494 yields 0xFF or func_800458E8 returns 0; if handle == 0 allocates via func_80046B04 (returning 0 on failure); computes pan via func_800460D4; calls func_8004772C and frees a newly allocated voice on failure.
 * @par Calls
 * func_80045494 - resolve track to bank slot; func_800458E8 - 2D/3D attenuation; func_80046B04 - allocate voice; func_800460D4 - compute stereo pan; func_8004772C - start voice; func_80046698 - free voice on failure.
 * @par Side effects
 * May allocate or free a voice in the sound pool and advance bank cursor/SPU playback via func_8004772C.
 * @par Callers
 * Confirmed: func_80025E18 (0x80025F08), func_80047088 (0x8004716C), func_800471C4 (0x80047258), func_8007CACC (0x8007CCEC, 0x8007CD90).
 */
s32 gPlaySoundIdOnVoice(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object)
{
    register s32 result __asm__("$16");
    register s32 slot __asm__("$17");
    struct SoundBank *bank;
    s8 mix[0x18];

    result = 0;
    __asm__("" : "=r"(result) : "0"(result));
    slot = gResolveSoundRecordIndex(track, gSoundBankTableLow, 0x47);
    __asm__("" : "=r"(slot) : "0"(slot));
    if (slot == 0xFF) {
        return result;
    }
    bank = &gSoundEntryTable[slot];
    if (gComputeSpatialSoundAttenuation(arg1, arg2, mix, object) == 0) {
        return result;
    }
    if (handle == 0) {
        result = gAllocPrioritySoundVoice(mix[0], bank->unk13, result, slot);
        if (result == 0) {
            goto done;
        }
        handle = result;
    }
    gComputeSpatialSoundPan(mix);
    if (gPlayNextBankSample(bank, handle, mix[1], mix[0], arg3) != 0) {
        return result;
    }
    if (result != 0) {
        gReleaseSoundVoice(result);
    }
    result = 0;
    __asm__("" : "=r"(result) : "0"(result));
done:
    return result;
}
