// Categories: Platform > Sound > Shutdown | Platform > Sound > Voices | Platform > SPU
#include "game/types.h"

struct Unk47554Node {
    u8 unk00[0xD];
    s8 voice;
    u8 unk0E[0xE];
    struct Unk47554Node *next;
};

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};

extern u8 gStreamAudioFlag __asm__("D_8009F739");
extern s32 D_8009F32C;
extern struct Unk47554Node *gVoicePriorityList __asm__("D_8009F744");
extern struct Unk47554Node *gVoiceList __asm__("D_8009F74C");
extern void *D_8009FA58[2];
extern struct Unk4749CHandle D_800A10C0[];

extern void func_80023E84(void);
extern void func_80024630(s32 arg);
extern void func_80047088(s32 arg0, void *arg1);
extern void func_80046698(void *arg);
extern void func_80021B90(void);
extern void func_80021BD0(s32 voice);
extern s32 func_800474F0(struct Unk4749CHandle *handle);

/**
 * @brief Halts active sound voices, music streams, and CD audio, optionally sparing one node.
 *
 * Clears D_8009F739, shuts down stream/CD audio via func_80023E84 (when node == 0) and
 * func_80024630(0), stops player track loops in D_8009FA58, releases queued handles,
 * frees priority 1/2 and priority 0 voices, and mutes SPU channels via func_80021BD0/B90.
 *
 * @param node Optional SoundVoice node in D_8009F74C to keep playing (others muted), or null to mute all.
 *
 * @par Globals
 * D_8009F739 - audio enable byte (cleared); D_8009FA58 - player objects; D_8009F32C/D_800A10C0 - tracked handles; D_8009F744/D_8009F74C - active voice lists.
 *
 * @par Control flow
 * Stops streams/CD audio, loops over 2 D_8009FA58 slots, drains D_800A10C0 and D_8009F744, walks D_8009F74C skipping node, and mutes SPU voices.
 *
 * @par Calls
 * func_80023E84, func_80024630 - stop stream/CD playback; func_80047088 - stop player track (0x27); func_800474F0, func_80046698 - release handles/voices; func_80021BD0, func_80021B90 - mute SPU.
 *
 * @par Side effects
 * Clears D_8009F739, stops streams and voices, frees voice nodes, and mutes SPU hardware channels.
 *
 * @par Callers
 * Confirmed: func_80047690 (0x800476B8, disable audio with arg 0), func_800476D0 (0x80047710, transition/pause audio).
 */
void func_80047554(struct Unk47554Node *node)
{
    s32 i;
    struct Unk47554Node *entry;
    struct Unk47554Node *next;

    gStreamAudioFlag = 0;
    if (node == 0) {
        func_80023E84();
    }
    func_80024630(0);
    {
        register void **p __asm__("$16");
        i = 0;
        p = D_8009FA58;
        do {
            if (*p != 0) {
                func_80047088(0x27, *p);
            }
            p++;
            i++;
        } while (i < 2);
    }
    while (D_8009F32C != 0) {
        D_8009F32C--;
        func_800474F0(&D_800A10C0[D_8009F32C]);
    }
    while (gVoicePriorityList != 0) {
        func_80046698(gVoicePriorityList);
    }
    entry = gVoiceList;
    while (entry != 0) {
        next = entry->next;
        if (entry != node) {
            func_80046698(entry);
        }
        entry = next;
    }
    if (node != 0) {
        func_80021BD0(node->voice);
    } else {
        func_80021B90();
    }
}
