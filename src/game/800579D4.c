// Categories: Gameplay > Object Spawning | Game Engine > Object Setup | Gameplay > Entity Update
#include "game/types.h"

struct Object;
struct Vec3 { s32 x, y, z; };
struct State {
    s16 values[16];
    s32 words[4];
    u8 pad30[0x54-0x30];
    u8 gauge[0xC1-0x54];
    s8 index;
    u8 padC2[2];
    struct Vec3 pos;
    u8 padD0[2];
    u16 unkD2;
    u8 padD4[0xDF-0xD4];
    u8 unkDF;
    u8 padE0[0x174-0xE0];
    s16 unk174, unk176;
    u8 model178[0x4C];
    u8 model1C4[0x4C];
    u8 model210[0x4C];
    u8 pad25C[0x2FE - 0x25C];
    s16 unk2FE;
    s32 unk300;
    u8 pad304[0x311-0x304];
    u8 unk311;
    u8 pad312;
    u8 unk313;
};
struct Obj28 { u8 pad[0x10]; void *anim; };
struct Object {
    u8 pad00[0x1C];
    s16 health, maxHealth;
    u8 pad20[4];
    u32 flags;
    struct Obj28 *obj28;
    u8 pad2C[0x50-0x2C];
    struct Vec3 pos;
    u8 pad5C[0x72-0x5C];
    u8 flags72;
    u8 pad73[0x8C-0x73];
    s32 unk8C;
    u8 pad90[0xAC-0x90];
    struct State *state;
    u8 padB0[6];
    u8 unkB6;
    u8 padB7[0xC4-0xB7];
    s32 unkC4;
    u8 anim[0xDC-0xC8];
    struct Object *child;
    void *modelE0, *modelE4, *modelE8, *modelEC;
};
struct Data8 { u8 pad00[0x24]; u16 unk24; s8 unk26, index; };
struct Data4 { u8 pad00[0x18]; void **p18; u8 pad1C[4]; void **p20; u8 pad24[4]; void **p28; u8 pad2C[4]; void **p30; };
struct Data { u8 pad00[4]; struct Data4 *d4; struct Data8 *d8; };
struct Game { u8 pad00[8]; s32 mode0; s32 mode; u8 pad10[0x5C-0x10]; s32 unk5C; u8 pad60[0x98-0x60]; s32 mode1; };
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern struct Object *D_8009FA58[];
extern struct Object *table_after[] __asm__("D_8009FA58+0");
extern s8 gPlayerInvulnerable[] __asm__("D_8009FA8C");
extern struct Game *D_8009FA94;
extern void *func_80017EBC(s32, s32);
extern void func_8002AD58(s32, struct Object *, struct Data *, s32);
extern void func_80044988(void *, void *, s32, s32);
extern void func_800573A4(struct Data8 *, struct Object *);
extern void func_80013BD8(void *, void *, s32);
extern struct Object *func_80057914(struct Object *);
extern s32 func_80067684(void *, s32);
extern void func_8005A0C4(void *, s32, s32);
extern void func_80051AF8(struct Object *);

/**
 * @brief Spawn (message 2) of the class-1 compound object: allocates the state
 *        block, registers the object in the player table, binds three models,
 *        spawns the child object and scales the health by the game mode.
 *
 * @param arg0 Object block: +0x1C health, +0x1E max health, +0x24 flags,
 *            +0x28 model holder, +0x50 position, +0x72 status byte, +0x8C and
 *            +0xB6 counters, +0xAC state block (written here), +0xC8 animation
 *            track, +0xDC child pointer, +0xE0/+0xE4/+0xE8/+0xEC the four model
 *            pointers.
 * @param arg1 Spawn record: +0x04 model table (four slots read at +0x18,
 *            +0x20, +0x28, +0x30), +0x08 spawn entry (u16 at +0x24, s8 ground
 *            mode at +0x26, s8 preferred slot at +0x27).
 * @param arg2 Forwarded unchanged to func_8002AD58 as its creation argument.
 * @return Nothing (declared void; the class-1 dispatcher ignores the result).
 *
 * @par Confirmed
 * 1. state = func_80017EBC(0x328, 2); the 16 s16 values at +0x00..+0x1F and the
 *    four words at +0x20..+0x2F are zeroed.
 * 2. func_8002AD58(2, obj, data, arg) runs the base create, then
 *    func_80044988(obj->+0x28->+0x10, obj + 0xC8, 1, 0) attaches the animation
 *    data in forward direction, +0x72 becomes (.. & 0xEF) | 0x20 and the global
 *    D_8009FA0C is incremented.
 * 3. Slot registration: the preferred slot from the record is kept only when
 *    D_8009FA58[slot] is free, otherwise the first free entry of that table is
 *    searched. The chosen slot goes to state +0xC1 and D_8009FA58[slot].
 * 4. func_800573A4(entry, obj) initialises the placement, +0xC4 = 0, and the
 *    four model pointers are read from the table slots (0 when a slot is null).
 * 5. Three model instances are bound with func_80013BD8: +0x178 and +0x1C4 from
 *    obj->+0xE4, +0x210 from obj->+0xE8.
 * 6. obj->+0x24 = (.. & 0xFEFFFFAE) | 0x40800300, then child = func_80057914(obj)
 *    receives flag 0x10 and both objects get flag 8.
 * 7. Ground placement: the s8 at +0x26 picks the query mode (0 gives -1, 1
 *    gives -2, n gives n - 2) and obj->+0x50.y becomes
 *    func_80067684(&obj->+0x50, mode) + 0x200, half a unit above the ground.
 *    The position is copied to state +0xC4 and the u16 at +0x24 to +0xD2.
 * 8. Health: value = obj->+0x1E; in two-player mode (D_8009FA94->mode == 2) with
 *    a zero unk5C it gains another 50 percent. The per-slot multiplier
 *    (D_8009FA94->mode0 for slot 0, mode1 otherwise) scales it by 100, 125 or
 *    150 percent for modes 0, 1 and 2; mode 3 forces the maximum - the value
 *    becomes 0x31FF9C and D_8009FA8C[slot] is set to 1, so the division by 100
 *    yields 0x7FFF. The result is clamped to 0x7FFF and stored in +0x1C and
 *    +0x1E.
 * 9. func_8005A0C4(state + 0x54, health, maxHealth) fills the gauge and
 *    func_80051AF8(obj) runs the compound reset, so the object is fully armed
 *    when the body returns.
 * The state block's +0x311, +0x313, +0x2FE, +0x300, +0x174 and +0x176 fields
 * are simply cleared here.
 *
 * @par Likely role
 * The per-player spawn of a compound craft: it books one of the two slots of
 * D_8009FA58, increases the global D_8009FA0C that selects the screen layout
 * (1 or 2 players), scales the hit points with the per-player difficulty in
 * D_8009FA94 and queues the companion object that handler D_80059ABC maintains.
 * Mode 3 reads as a "maximum health" variant rather than a multiplier.
 *
 * @par Unknown
 * What D_8009FA8C stores (a two-entry per-player byte array that several message
 * handlers test before acting) and what D_8009FA94->unk5C gates in the
 * two-player bonus.
 *
 * @par Callers
 * func_8005908C only, the class-1 handler installed by func_8002AE54, which
 * calls this body for message 2.
 */
void func_800579D4(struct Object *arg0, struct Data *arg1, s32 arg2)
{
    register struct Object *obj __asm__("$16");
    struct State *state;
    register struct Data *data __asm__("$18");
    register s32 arg __asm__("$19");
    register s32 i __asm__("$3");
    s32 index, ground, selector, value, mode;
    struct Data4 *d4;
    s16 *p;
    s32 *q;
    s32 *modePtr;
    obj = arg0;
    data = arg1;
    arg = arg2;
    __asm__ ("" : "+r" (obj), "+r" (data), "+r" (arg));
    obj->state = func_80017EBC(0x328, 2);
    i = 15;
    state = obj->state;
    p = &state->values[15];
    for (; i >= 0; i--) *p-- = 0;
    i = 3;
    q = (s32 *)state + 3;
    for (; i >= 0; i--) { q[8] = 0; q--; }
    func_8002AD58(2, obj, data, arg);
    func_80044988(obj->obj28->anim, obj->anim, 1, 0);
    obj->flags72 = (obj->flags72 & 0xEF) | 0x20;
    gPlayerCount++;
    index = data->d8->index;
    if (index != -1) {
        register s32 offset __asm__("$3") = index * 4;
        __asm__ volatile ("" : "+r" (offset));
        if (*(struct Object **)((u8 *)D_8009FA58 + offset) == 0) goto found;
    }
    index = 0;
    if (D_8009FA58[0] != 0) {
        register struct Object **p __asm__("$3") = table_after;
        do { p++; index++; } while (*p != 0);
    }
found:
    __asm__ volatile ("" : "+r" (index));
    D_8009FA58[index] = obj;
    state->index = index;
    state->unkDF = 0;
    func_800573A4(data->d8, obj);
    obj->unkC4 = 0;
    d4 = data->d4;
    if (d4->p18) obj->modelE0 = *d4->p18; else obj->modelE0 = 0;
    if (d4->p20) obj->modelE4 = *d4->p20; else obj->modelE4 = 0;
    if (d4->p28) obj->modelE8 = *d4->p28; else obj->modelE8 = 0;
    if (d4->p30) obj->modelEC = *d4->p30; else obj->modelEC = 0;
    state->unk176 = 0;
    state->unk174 = 0;
    func_80013BD8(state->model178, obj->modelE4, 0);
    func_80013BD8(state->model1C4, obj->modelE4, 0);
    func_80013BD8(state->model210, obj->modelE8, 0);
    obj->flags = (obj->flags & 0xFEFFFFAE) | 0x40800300;
    obj->child = func_80057914(obj);
    obj->child->flags |= 0x10;
    obj->flags |= 8;
    obj->child->flags |= 8;
    obj->unkB6 = 0;
    obj->unk8C = 0;
    selector = data->d8->unk26;
    if (selector != 0) {
        if (selector != 1) ground = selector - 2;
        else ground = -2;
    } else ground = -1;
    obj->pos.y = func_80067684(&obj->pos, ground) + 0x200;
    state->pos = obj->pos;
    state->unkD2 = data->d8->unk24;
    state->unk313 = 0;
    state->unk311 = 0;
    state->unk300 = 0;
    state->unk2FE = 0;
    value = obj->maxHealth;
    if (D_8009FA94->mode == 2 && D_8009FA94->unk5C == 0) value += value / 2;
    if (obj->state->index) modePtr = &D_8009FA94->mode1;
    else modePtr = &D_8009FA94->mode0;
    mode = *modePtr;
    switch (mode) {
    case 2: value *= 150; break;
    case 1: value *= 125; break;
    case 0: value *= 100; break;
    case 3: {
        register s32 special __asm__("$7") = 0x310000;
        register struct State *s __asm__("$2") = obj->state;
        register s32 idx __asm__("$2");
        __asm__ volatile ("" : "+r" (special) : "r" (s));
        special |= 0xFF9C;
        idx = s->index;
        __asm__ volatile ("" : "+r" (idx) : "r" (special));
        gPlayerInvulnerable[idx] = 1;
        value = special;
        break;
    }
    }
    value /= 100;
    if (value > 0x7FFF) value = 0x7FFF;
    obj->health = value;
    obj->maxHealth = value;
    func_8005A0C4(state->gauge, obj->health, obj->maxHealth);
    func_80051AF8(obj);
}
