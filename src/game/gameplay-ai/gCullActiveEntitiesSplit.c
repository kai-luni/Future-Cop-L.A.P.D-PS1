// Categories: Gameplay > Entity Update | Platform > Sound | Game Engine > Frame
#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 unk04;
    s32 id;
    u8 pad0C[0x0A];
    u8 unk16;
    u8 pad17[0x0A];
    u8 unk21;
    s8 unk22;
    u8 pad23;
    s32 flags;
    u8 pad28[0x28];
    s32 x;
    u8 pad54[4];
    s32 z;
};

extern struct Entity *gPlayerEntities __asm__("D_8009FA58");
extern struct Entity *gPlayerEntity2 __asm__("D_8009FA5C");
extern struct Entity *gFocusPositions __asm__("D_8009F9A0");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern s32 gProximityScriptDeadline __asm__("D_8009F7A4");
extern struct Entity *gUnitListHead __asm__("D_8009FA40");
extern s32 gCommandListHead __asm__("D_800A8F90");
extern s32 gEffectPoolCapacity __asm__("D_800A8F94");
extern struct Entity *gEffectListHead[] __asm__("D_800A8F98");
extern s32 gAllocPositionalSoundChannel(s32, s32, s32, s32, s32) __asm__("func_80045678");
extern void gSetSoundChannelPosition(s32, s32, s32, s32, s32) __asm__("func_8004572C");
extern void gStopSoundChannelByIndex(s32, s32) __asm__("func_800457B8");
extern void gRunEntityScript(struct Entity *) __asm__("func_8004DEE0");
void gCullActiveEntitiesSplit(void) __asm__("func_8004E248");


/**
 * @brief Culls world entities in D_8009FA40 by proximity, builds active list D_800A8F98, updates entity sounds, and triggers one script.
 *
 * Walks D_8009FA40 measuring octagonal distance to D_8009FA58/D_8009FA5C (or override D_8009F9A0),
 * shrinks limit from 0x38000 down toward 0x20000 if count reaches capacity D_800A8F94, maintains
 * active flag 0x20000000 and ambient channel flag 0x400000, stores count in D_800A8F90, and runs the nearest 0x100000 script.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA58/D_8009FA5C/D_8009F9A0 - distance reference entities; D_8009FA40 - master entity list;
 * D_800A8F90/D_800A8F94/D_800A8F98 - active count, capacity, and output array; D_8009FAA4/D_8009F7A4 - tick and 0x12C-tick script cooldown.
 * @par Control flow
 * Outer retry loop reducing limit by 0x4000 on overflow; inner do-while over D_8009FA40 testing octagonal distance against 0x21FFF and limit.
 * @par Calls
 * func_80045678 - starts looping entity sound (stores channel in unk22); func_8004572C - updates sound coords;
 * func_800457B8 - stops channel when out of range/inactive; func_8004DEE0 - runs closest bestEnt->script once per 0x12C ticks.
 * @par Side effects
 * Rewrites D_800A8F90/D_800A8F98, D_8009F7A4, entity flags/unk22, active sound channels, and runs one entity script.
 * @par Callers
 * Confirmed: func_80027510 (two-player unpaused frame loop).
 */
void gCullActiveEntitiesSplit(void)
{
    s32 flag;
    s32 x, z;
    register struct Entity *e __asm__("$16");
    register s32 limit __asm__("$17");
    register s32 count __asm__("$18");
    struct Entity **out;
    register s32 mask __asm__("$20");
    register s32 bz __asm__("$21");
    register s32 bx __asm__("$22");
    register s32 best __asm__("$23");
    struct Entity *bestEnt;
    register s32 ex __asm__("$5");
    register s32 ez __asm__("$8");
    register s32 d __asm__("$7");
    register s32 dz __asm__("$4");
    register s32 m __asm__("$6");
    register s32 v0 __asm__("$2");
    register s32 v1 __asm__("$3");

    x = gPlayerEntities->x;
    bestEnt = 0;
    z = gPlayerEntities->z;
    bx = gPlayerEntity2->x;
    bz = gPlayerEntity2->z;
    best = 0x40000000;
    if (gFocusPositions != 0) {
        bx = gFocusPositions->x;
        bz = gFocusPositions->z;
        x = bx;
        z = bz;
    }
    flag = 0;
    limit = 0x38000;
outer_head:
    if (gProximityScriptDeadline <= gFrameTime) {
        flag = 1;
        gProximityScriptDeadline = gFrameTime + 0x12C;
    }
    out = gEffectListHead;
    count = 0;
    e = gUnitListHead;
    if (e != 0) {
        mask = 0x400000;
        do {
            if (e->id != 0) {
                ex = e->x;
                ez = e->z;
                d = ex - x;
                v1 = d >> 31;
                v0 = v1 + d;
                d = v0 ^ v1;
                dz = ez - z;
                v1 = dz >> 31;
                v0 = v1 + dz;
                dz = v0 ^ v1;
                m = d;
                if (dz < d) m = dz;
                d = d + dz;
                v0 = m / 2;
                d = d - v0;
                if (d > 0x21FFF) {
                    ex = ex - bx;
                    v1 = ex >> 31;
                    v0 = v1 + ex;
                    ex = v0 ^ v1;
                    dz = ez - bz;
                    v1 = dz >> 31;
                    v0 = v1 + dz;
                    dz = v0 ^ v1;
                    m = ex;
                    if (dz < ex) m = dz;
                    ex = ex + dz;
                    v0 = m / 2;
                    ex = ex - v0;
                    if (ex < d) d = ex;
                }
                {
                    s32 flags = e->flags;
                    s32 nf;
                    if ((flags & 8) == 0 && d >= limit) goto inactive;
                    nf = (flags | 0x20000000) & 0xFDFFFFFF;
                    e->flags = nf;
                    if (d <= 0x21FFF) {
                        if (flag != 0 && (nf & 0x100000) != 0 && e->unk16 != 0x24 && d < best) {
                            best = d;
                            bestEnt = e;
                        }
                        if (e->unk21 != 0) {
                            if ((e->flags & mask) == 0) {
                                s8 idx = gAllocPositionalSoundChannel(e->unk21, e->id, e->x, e->z, -1);
                                e->unk22 = idx;
                                if (idx >= 0) e->flags |= mask;
                            } else {
                                gSetSoundChannelPosition(e->id, e->x, e->z, -1, e->unk22);
                            }
                        }
                    } else {
                        if (nf & mask) {
                            gStopSoundChannelByIndex(e->id, e->unk22);
                            e->flags &= 0xFFBFFFFF;
                        }
                    }
                    v0 = 0x800B0000;
                    __asm__("" : "+r"(v0));
                    if (count >= *(s32 *)(v0 - 0x706C)) {
                        limit -= 0x4000;
                        if (limit > 0x20000) goto outer_head;
                        goto done;
                    }
                    count++;
                    *out++ = e;
                    goto next;
                inactive:
                    v0 = flags & 0xDFFFFFFF;
                    e->flags = v0;
                    __asm__("" : "+r"(v0) : : "memory");
                    if (v0 & mask) {
                        gStopSoundChannelByIndex(e->id, e->unk22);
                        e->flags &= 0xFFBFFFFF;
                    }
                }
            next:;
            }
        } while ((e = e->next) != 0);
    }
done:
    v0 = 0x800B0000;
    __asm__("" : "+r"(v0));
    *(s32 *)(v0 - 0x7070) = count;
    if (bestEnt != 0) gRunEntityScript(bestEnt);
}
