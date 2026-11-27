// Categories: Graphics > Effects > Beams | Game Engine > Frame
#include "game/types.h"

struct Unk71A68Entry {
    void *data;
    void *extra;
    s32 unk08;
    u8 unk0C;
    u8 unk0D;
    u8 unk0E;
    u8 unk0F;
};

struct Unk71A68 {
    s32 unk00;
    void *first;
    void *second;
    s16 count;
    u16 unk0E;
    struct Unk71A68Entry *entries;
    u8 pad14[0x12];
    s8 unk26;
};

extern s32 func_80019668(void);
extern void gBuildRibbonBeamStrip(struct Unk71A68 *object, struct Unk71A68Entry *entry, s32 arg2) __asm__("func_80070E48");
s32 gStepBeamEntryTimers(struct Unk71A68 *object_, s32 a1, s32 a2, s32 a3, s32 a4) __asm__("func_800718B8");


/**
 * @brief Ticks one beam entry: drains its three timers and re-initialises
 *        or finishes the beam under the blink gate.
 *
 * @param object Beam owner; +0xE the mode flags, +0x26 the repeat counter.
 * @param a1 Entry index into object->entries.
 * @param a2, a3, a4 Frame deltas for the timers +0xC/+0xD/+0xE.
 * @return Non-zero while any of the three timers is still running.
 *
 * @par Confirmed
 * Each non-zero timer is decremented by its delta, clamped at 0. The
 * return value ORs the three "still running" flags (each only when its
 * delta was non-zero). With mode bit 0x8000 set: the frame parity
 * func_80019668() & 1 or a cleared bit 2 opens the gate; bit 0x2000
 * inverts the condition. Inside, unk26 <= 0 or --unk26 > 0 calls
 * func_80070E48(object, entry, 1) (re-init); otherwise the mode becomes
 * (mode & ~2) | 1 (finished).
 *
 * @par Likely role
 * The per-frame driver of the beam list func_80071B40: the three timers
 * are likely fade-in/hold/fade-out phases, the blink gate flickers the
 * beam on alternate frames, and unk26 counts the repeats.
 *
 * @par Unknown
 * The exact phase meaning of the three timers.
 *
 * @par Callers
 * func_80071B40.
 */
s32 gStepBeamEntryTimers(struct Unk71A68 *object_, s32 a1, s32 a2, s32 a3, s32 a4)
{
    register struct Unk71A68 *object __asm__("$17") = object_;
    register s32 a1_reg __asm__("$5");
    struct Unk71A68Entry *entry;
    s32 res;
    u8 cond;

    entry = &object->entries[a1];
    a1_reg = a4;

    if (entry->unk0E != 0) {
        if (entry->unk0E < a1_reg) {
            entry->unk0E = 0;
        } else {
            entry->unk0E -= a1_reg;
        }
    }

    if (entry->unk0D != 0) {
        if (entry->unk0D < a3) {
            entry->unk0D = 0;
        } else {
            entry->unk0D -= a3;
        }
    }

    if (entry->unk0C != 0) {
        if (entry->unk0C < a2) {
            entry->unk0C = 0;
        } else {
            entry->unk0C -= a2;
        }
    }

    
    {
        register s32 a0 __asm__("$4");
        register s32 v1 __asm__("$3");
        register s32 v0 __asm__("$2");

        a0 = 0;
        if (a2) {
            v0 = entry->unk0C != 0;
            __asm__ volatile ("" : "+r"(v0));
            a0 = v0;
        }
        v0 = 0;
        if (a3) {
            v0 = entry->unk0D != 0;
        }
        v1 = 0;
        a0 |= v0;
        if (a1_reg) {
            v0 = entry->unk0E != 0;
            __asm__ volatile ("" : "+r"(v0));
            v1 = v0;
        }
        res = v1 | a0;
    }


    if (object->unk0E & 0x8000) {
        if ((func_80019668() & 1) != 0 || !(object->unk0E & 2)) {
            cond = 1;
            if (object->unk0E & 0x2000) {
                cond = res ^ 1;
            }
            if (cond) {
                if (object->unk26 <= 0 || --object->unk26 > 0) {
                    gBuildRibbonBeamStrip(object, entry, 1);
                } else {
                    object->unk0E = (object->unk0E & ~2) | 1;
                }
            }
        }
    }

    return res;
}
