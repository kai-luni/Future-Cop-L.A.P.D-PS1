// Categories: Platform > Sound > Music | Input > Menu Navigation
#include "game/types.h"

struct Sel {
    s8 mode;
    u8 pad1[4];
    s8 state;
};

struct Voice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern u8 gSnapshotResult __asm__("D_8009F904");
extern struct Sel *gFontBankTable __asm__("D_8009F468");
extern struct Voice *gSnapshotPointer __asm__("D_8009F91C");
extern s32 gPreviewTrackId __asm__("D_8009F920");

extern s32 func_8004522C(void);
extern void gSetSfxVolume(s32 volume) __asm__("func_800451F8");
extern s32 func_80045288(void);
extern void gSetVagmVolumeSetting(s32 volume) __asm__("func_80045238");
extern s32 func_800452C8(void);
extern void func_80045294(s32 volume);
extern void gStopSoundVoiceHandle(s32 handle) __asm__("func_80046A34");
extern s32 gAllocVoiceForSoundId(s32 arg0, s32 arg1, s32 arg2, s32 id) __asm__("func_80046A58");
extern s32 gPlaySoundIdOnVoice(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object) __asm__("func_80046E88");
extern void func_80047800(struct Voice *voice, s32 pan, s32 volume, s32 extra);

/**
 * @brief Menu-driven music preview and volume callback.
 *
 * Runs while the front-end menu at D_8009F468 is in state 0 (or state 1 with
 * arg1 zero). Flag 0x40000000 stores arg1 in D_8009F904. Flags 0x800000 and
 * 0x200000 step the volume of the active menu mode down or up through the
 * getter/setter pairs (mode 1 music, mode 2 effects, mode 3 third channel).
 * Menu mode 1 maps to preview track 0x4A and mode 3 to track 0x4B: a stale
 * voice for another track is stopped, a missing voice is allocated with
 * func_80046A58 and started with func_80046E88, and an existing voice is
 * refreshed with func_80047800 at the current mode volume. Any other menu
 * mode stops the preview voice. Returns flags unchanged.
 *
 * @param flags Command bits: 0x40000000 store, 0x800000 volume down,
 *        0x200000 volume up.
 * @param arg1 Value stored under flag 0x40000000; also gates state 1.
 * @return The flags argument, unmodified.
 *
 * @par Globals
 * D_8009F468 - menu block (mode byte +0x00, state byte +0x05); D_8009F91C -
 * preview voice handle; D_8009F920 - track id the voice was started for;
 * D_8009F904 - stored arg1 value.
 *
 * @par Control flow
 * State gate, two volume switches on menu mode, then a three-way branch
 * (track 0x4A / track 0x4B / stop) with stop-and-replace voice management.
 *
 * @par Calls
 * func_8004522C/func_800451F8, func_80045288/func_80045238,
 * func_800452C8/func_80045294 - volume get/set pairs; func_80046A34 - stop
 * voice; func_80046A58 - allocate voice; func_80046E88 - start track;
 * func_80047800 - update voice pan/volume.
 *
 * @par Side effects
 * Writes D_8009F904, the three volume words, and the voice handle/track id.
 *
 * @par Callers
 * Confirmed: none direct. Likely: invoked through the callback slot +0xC
 * that func_80077AD4 installs (pointer load at 0x80077D88). Unknown: the
 * dispatcher that calls the slot.
 */
s32 gHandleMusicPreviewMenu(s32 flags, s32 arg1) __asm__("func_8007CACC");

s32 gHandleMusicPreviewMenu(s32 flags, s32 arg1)
{
    s32 mode;
    register s32 k __asm__("$2");

    mode = 0;
    if (flags & 0x40000000) {
        gSnapshotResult = arg1;
    }
    if (gFontBankTable->state == 0 || (gFontBankTable->state == 1 && arg1 == 0)) {
        if (gFontBankTable->mode == 1) {
            mode = 1;
        } else if (gFontBankTable->mode == 3) {
            mode = 2;
        }
        if (flags & 0x800000) {
            switch (gFontBankTable->mode) {
            case 1:
                gSetSfxVolume(func_8004522C() - 1);
                break;
            case 2:
                gSetVagmVolumeSetting(func_80045288() - 1);
                break;
            case 3:
                func_80045294(func_800452C8() - 1);
                break;
            }
        } else if (flags & 0x200000) {
            switch (gFontBankTable->mode) {
            case 1:
                gSetSfxVolume(func_8004522C() + 1);
                break;
            case 2:
                gSetVagmVolumeSetting(func_80045288() + 1);
                break;
            case 3:
                func_80045294(func_800452C8() + 1);
                break;
            }
        }
        if (mode == 1) {
            if (gSnapshotPointer != 0) {
                if (gPreviewTrackId != 0x4A) {
                    gStopSoundVoiceHandle((s32)gSnapshotPointer);
                    gSnapshotPointer = 0;
                }
            }
            if (gSnapshotPointer == 0) {
                gSnapshotPointer = (struct Voice *)gAllocVoiceForSoundId(func_8004522C(), 10, 0, 0x4A);
                if (gSnapshotPointer == 0) {
                    return flags;
                }
                gPlaySoundIdOnVoice(0x4A, 0, 0, -1, (s32)gSnapshotPointer, 0);
                k = 0x4A;
                gPreviewTrackId = k;
                return flags;
            }
            func_80047800(gSnapshotPointer, gSnapshotPointer->pan, func_8004522C(), -1);
        } else if (mode == 2) {
            if (gSnapshotPointer != 0) {
                if (gPreviewTrackId != 0x4B) {
                    gStopSoundVoiceHandle((s32)gSnapshotPointer);
                    gSnapshotPointer = 0;
                }
            }
            if (gSnapshotPointer == 0) {
                gSnapshotPointer = (struct Voice *)gAllocVoiceForSoundId(func_800452C8(), 10, 0, 0x4B);
                if (gSnapshotPointer == 0) {
                    return flags;
                }
                gPlaySoundIdOnVoice(0x4B, 0, 0, -1, (s32)gSnapshotPointer, 0);
                k = 0x4B;
                gPreviewTrackId = k;
                return flags;
            }
            func_80047800(gSnapshotPointer, gSnapshotPointer->pan, func_800452C8(), -1);
        } else {
            if (gSnapshotPointer != 0) {
                gStopSoundVoiceHandle((s32)gSnapshotPointer);
                gSnapshotPointer = 0;
            }
        }
    }
    return flags;
}
