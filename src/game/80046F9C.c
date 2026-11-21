// Categories: Platform > Sound > SE | Platform > Sound > Voice Allocation
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

extern volatile u8 gSoundProcessingEnabled __asm__("D_8009F340");
extern struct SoundBank gSoundEntryTable[] __asm__("D_800A7938");

extern s32 gComputeSpatialSoundAttenuation(s32 arg0, s32 arg1, s8 *mix, s32 arg3) __asm__("func_800458E8");
extern s32 gAllocPrioritySoundVoice(s32 volume, s32 arg1, s32 arg2, u32 index) __asm__("func_80046B04");
extern void gComputeSpatialSoundPan(s8 *mix) __asm__("func_800460D4");
extern u8 func_8004772C(struct SoundBank *bank, s32 handle, s32 pan, s32 volume, s32 extra);
extern void gReleaseSoundVoice(s32 handle) __asm__("func_80046698");
s32 gPlaySpatialSoundBankSlot(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4) __asm__("func_80046F9C");


/**
 * @brief Starts a sound bank entry as a positional sound effect and returns its voice handle, or 0 when any guard fails.
 *
 * @param arg0 Source x coordinate; (0,0) means "at the listener" in the mix computation.
 * @param arg1 Source z coordinate.
 * @param index Bank slot in D_800A7938; must be below 0x6E.
 * @param arg3 Entity pointer for the mix computation; the +0xC1 mode byte of its +0xAC stats block can force hard-left/right pan.
 * @param arg4 Voice class byte (cast to s8) for the allocator; class 1 raises priority by 0x64.
 * @return Allocated voice handle, or 0 on disabled sound, bad index, empty bank, inaudible position, allocation or start failure.
 *
 * @par Globals
 * D_8009F340 - sound enable flag, checked first. D_800A7938 - 0x6E-entry bank table; count gates playback, unk13 feeds allocator priority, cursor advances in the starter.
 *
 * @par Control flow
 * Guard chain of early returns, then allocate-and-start with a rollback branch: a failed func_8004772C frees the voice and forces handle 0.
 *
 * @par Calls
 * func_800458E8 - fills the 0x18-byte stack mix (volume/pan/flag plus listener deltas); 0 = inaudible. func_80046B04 - voice allocation with priority steal. func_800460D4 - pan from listener angle when the mix flag is 0. func_8004772C - round-robin sample start. func_80046698 - voice release on failure.
 *
 * @par Side effects
 * Links a voice into the active lists, advances the bank cursor and starts SPU playback through the starter; undoes the allocation on failure.
 *
 * @par Callers
 * Confirmed: func_80046C84 (SE by id, position 0,0, class 1), func_80046CF0 (looping weapon sound, class 0), func_80047284 (positional one-shot, class 1).
 */
s32 gPlaySpatialSoundBankSlot(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4)
{
    struct SoundBank *bank;
    register s32 handle __asm__("$17");
    register u32 indexValue __asm__("$18");
    register s32 arg4Value __asm__("$19");
    register u32 enabled __asm__("$2");
    s8 mix[0x18];

    enabled = gSoundProcessingEnabled;
    arg4Value = arg4;
    __asm__("" : "=r"(arg4Value) : "0"(arg4Value));
    indexValue = index;
    __asm__("" : "=r"(indexValue) : "0"(indexValue));
    handle = 0;
    __asm__("" : "=r"(handle) : "0"(handle));
    if (enabled == 0) {
        return handle;
    }
    if (indexValue >= 0x6E) {
        return handle;
    }
    bank = &gSoundEntryTable[indexValue];
    if (bank->count == 0) {
        return handle;
    }
    if (gComputeSpatialSoundAttenuation(arg0, arg1, mix, arg3) == 0) {
        return handle;
    }
    handle = gAllocPrioritySoundVoice(mix[0], bank->unk13, (s8)arg4Value, indexValue);
    if (handle != 0) {
        gComputeSpatialSoundPan(mix);
        if (func_8004772C(bank, handle, mix[1], mix[0], -1) == 0) {
            gReleaseSoundVoice(handle);
            handle = 0;
        }
    }
    return handle;
}
