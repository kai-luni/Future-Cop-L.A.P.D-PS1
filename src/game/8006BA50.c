// Categories: Gameplay > Weapons > Input Weapon Slot | Graphics > Effects > Dispatcher | Platform > Sound
#include "game/types.h"

extern int gWeaponSlotInput();

struct Unk6BA50Node {
    struct Unk6BA50Node *next;
    u8 unk04[4];
    s32 unk08;
    u8 unk0C[8];
    u16 unk14;
    u8 unk16;
    u8 unk17[5];
    s16 unk1C;
    u8 unk1E[0xA6];
    s32 unkC4;
};

struct Unk6BA50Ctl {
    u8 unk00[0x78];
    s32 unk78[3];
    s16 unk84[3];
    s16 unk8A[3];
    u8 unk90[3];
    u8 unk93[3];
    u8 unk96[2];
    struct Unk6BA50Node *unk98;
    s32 unk9C;
    u8 unkA0[0x21];
    s8 unkC1;
};

struct Unk6BA50Obj {
    u8 unk00[0xAC];
    struct Unk6BA50Ctl *ctl;
};

struct Unk6BA50Entry {
    u8 type;
    u8 unk01[3];
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    s16 unk08;
    s16 unk0A;
    u8 unk0C[0x13];
    u8 unk1F;
};

struct Unk6BA50Tgt {
    u8 unk00[0xA];
    u8 unk0A;
    u8 unk0B[0x39];
    s32 unk44;
    u8 unk48[8];
    s32 unk50;
};

extern struct Unk6BA50Node *gUnitListHead __asm__("D_8009FA40");
extern u8 gPlayerInvulnerable[] __asm__("D_8009FA8C");

extern s32 gPlayNonpositionalSoundEffect(s32 id, struct Unk6BA50Obj *obj) __asm__("func_80046C84");
extern struct Unk6BA50Tgt *gRegistryFind(u8 id) __asm__("func_8006AAEC");
extern s32 gWeaponFireDispatch(struct Unk6BA50Obj *obj, s32 idx) __asm__("func_8006ACB0");
extern void gBeamPartUpdate(struct Unk6BA50Obj *obj, s32 idx, s32 arg2) __asm__("func_8006B23C");

#define S16(off) (*(s16 *)((u32)ctl + (idx << 1) + (off)))

/**
 * @brief Drives one weapon slot of the player vehicle for one frame: handles the
 *        slot-2 target bookkeeping, refills or toggles the shot counter, then
 *        either fires the weapon or updates the beam part.
 *
 * @param obj Owner block; +0xAC is the control block (ctl below).
 * @param idx Slot index, only 0..2 are accepted. gWeaponSlotInput maps the pad to
 *            the slots: 0 for button 0x04000000, 1 for button 0x00400000, 2 on
 *            the weapon-switch edge.
 * @return Nothing.
 *
 * @par Confirmed
 * ctl (obj->+0xAC) is read as +0x78 reload timers, +0x84 per-slot counter,
 * +0x8A shared counter, +0x90 per-slot definition id, +0x93 per-slot cycle
 * counter, +0x98 pending node, +0x9C its expected id and +0xC1 owner slot.
 * 1. idx >= 3 returns at once. For idx == 2 a pending node is validated against
 *    +0x9C; the list D_8009FA40 is then walked and every node with a non-zero
 *    +0x08 and the same +0x14/+0x16 pair is retired: +0x16 == 0xF clears
 *    +0xC4 = 0, every other value (7 included) sets +0x1C = -1. With a matching
 *    pending node the pointer is then cleared and
 *    the function returns, so slot 2 is the "retire the old weapon" pass; when
 *    there is none, or the id no longer matches, the normal path below runs.
 * 2. A positive reload timer at +0x78 idx returns early (still reloading).
 * 3. The counter comes from the shared +0x8A when it is positive (flag set),
 *    otherwise from +0x84 idx. A value <= 0 plays SE 0xD through
 *    func_80046C84 and returns.
 * 4. A value of 5 plays SE 0xE (func_80046C84) and continues.
 * 5. The definition record gRegistryFind(+0x90 idx) is read at +0x24 when the
 *    flag is set, else at +0x04. Entry types 10..12 and 14 go to
 *    gBeamPartUpdate(obj, idx, entry + 0x1F), the beam/part update, and everything
 *    else to gWeaponFireDispatch(obj, idx), the weapon fire.
 * 6. When the sub-call reports success the counter is written back: it is
 *    decremented when D_8009FA8C[+0xC1] is 0 and toggled in bit 0 otherwise; the
 *    target is +0x8A or +0x84 idx. The cycle counter +0x93 idx is then
 *    decremented and either reloaded from entry +0x06 (adding entry +0x08 to the
 *    reload timer +0x78 idx) or left running (adding entry +0x0A).
 * 7. In the shared-counter case a counter that reached 0 is cleared, the cycle
 *    counter is reloaded from the record's +0x0A and the record's +0x44 and
 *    +0x50 words are swapped - the alternating muzzle halves.
 *
 * @par Likely role
 * The trigger logic of the vehicle weapons: entry +0x06/+0x08 read as a periodic
 * refire and +0x0A as the alternating rate, the shared counter as the linked
 * firing pattern of one slot, and D_8009FA8C[owner slot] as the per-player switch
 * that decides between counting down and toggling (single versus paired fire).
 *
 * @par Unknown
 * What D_8009FA8C stores, what the +0xC4 field cleared for node type 0xF means,
 * and why slot 2 consumes the pending node instead of firing.
 *
 * @par Callers
 * gWeaponSlotInput only, which calls it three times per frame (slots 0, 1 and 2).
 */
void gWeaponSlotUpdate(struct Unk6BA50Obj *obj, s32 idx) __asm__("func_8006BA50");

void gWeaponSlotUpdate(struct Unk6BA50Obj *obj, s32 idx)
{
    struct Unk6BA50Ctl *ctl;
    struct Unk6BA50Node *n;
    struct Unk6BA50Tgt *tgt;
    struct Unk6BA50Entry *e;
    s32 val;
    u8 flag;
    s32 ty;
    register s32 t __asm__("$2");
    register s32 t2 __asm__("$3");
    register s32 c __asm__("$2");
    u8 ok;

    ok = 1;
    if ((u32)idx < 3) {
        ctl = obj->ctl;
        if (idx == 2) {
            n = ctl->unk98;
            if (n != 0) {
                if (ctl->unk9C != n->unk08) {
                    ctl->unk98 = 0;
                } else {
                    struct Unk6BA50Node *m = gUnitListHead;
                    u32 a = n->unk14;
                    u32 b = n->unk16;
                    for (; m != 0; m = m->next) {
                        if (m->unk08 != 0 && m->unk14 == a && m->unk16 == b) {
                            if (m->unk16 != 7) {
                                if (m->unk16 == 0xF) {
                                    m->unkC4 = 0;
                                } else {
                                    m->unk1C = -1;
                                }
                            } else {
                                m->unk1C = -1;
                            }
                        }
                    }
                    ctl->unk98 = 0;
                    return;
                }
            }
        }
        if (ctl->unk78[idx] > 0) {
            return;
        }
        val = S16(0x8A);
        flag = val > 0;
        if (!flag) {
            val = ctl->unk84[idx];
            if (val <= 0) {
                gPlayNonpositionalSoundEffect(0xD, obj);
                return;
            }
        }
        if (val == 5) {
            gPlayNonpositionalSoundEffect(0xE, obj);
        }
        tgt = gRegistryFind(ctl->unk90[idx]);
        e = (struct Unk6BA50Entry *)((u8 *)tgt + (flag ? 0x24 : 0x4));
        ty = e->type;
        if (ty >= 10 && (ty < 13 || ty == 14)) {
            gBeamPartUpdate(obj, idx, e->unk1F);
        } else {
            ok = gWeaponFireDispatch(obj, idx);
        }
        if (ok) {
            c = ctl->unkC1;
            __asm__ volatile("" : "+r"(c));
            if (gPlayerInvulnerable[c] == 0) {
                val = val - 1;
            } else {
                val = val ^ 1;
            }
            if (flag) {
                S16(0x8A) = val;
            } else {
                ctl->unk84[idx] = val;
            }
            if (--ctl->unk93[idx] == 0) {
                ctl->unk93[idx] = e->unk06;
                ctl->unk78[idx] += e->unk08;
            } else {
                ctl->unk78[idx] += e->unk0A;
            }
            if (flag && val <= 0) {
                S16(0x8A) = 0;
                ctl->unk93[idx] = tgt->unk0A;
                t = tgt->unk50;
                t2 = tgt->unk44;
                tgt->unk44 = t;
                tgt->unk50 = t2;
            }
        }
    }
}
