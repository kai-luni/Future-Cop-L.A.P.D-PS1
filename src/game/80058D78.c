// Categories: Gameplay > Entity Update | Graphics > Effects > Object-Attached Effect | Game Engine > Entity Lifecycle
#include "game/types.h"

typedef struct Obj Obj;
typedef struct Data Data;
struct Data { u8 p0[0x14]; u16 count14, count16; u8 p18[8]; void *ptr20; u8 p24[0x9d]; u8 team; u8 pc2[0x1c]; u8 flag; u8 pdf[0x239]; u16 timer; };
struct Obj { u8 p0[4]; void (*update)(); u32 flags8; u8 pc[8]; u16 tag; u8 p16[6]; s16 angle, other; u8 active; u8 p21[3]; u32 flags24; u8 p28[0x64]; u32 id; u8 p90[0x1c]; Data *data; u8 pb0[0x2c]; Obj *child; u8 pe0[0xc]; void *owner; u8 pf0[0x44]; void *stats; };
struct Mode { u8 p[0x5c]; int active; };
struct Stats { u8 p[0x18]; u16 a; u8 p1a[0x22]; u16 b; };
extern struct Mode *gGameSession __asm__("D_8009FA94");
extern void *gFrameTime __asm__("D_8009FAA4");
extern int gScreenState __asm__("D_8009FA00");
extern int gPlayerCount __asm__("D_8009FA0C");
extern Obj *gPlayerEntities[] __asm__("D_8009FA58");
extern Obj *gSinglePlayerEntity __asm__("D_8009F9EC");
extern void D_800578B4(), D_800578E4();
extern int gRetireEntityBase(Obj *, int, int) __asm__("func_8002A810");
extern void func_80058C3C(void *, void *, Obj *);
extern void gSpawnTypeAtOwner(Obj *, int, int, int, int) __asm__("func_800402DC");
extern void func_80046C84(int, Obj *);
extern void gFlagQuartileChange(Obj *, int) __asm__("func_8003BA50");
extern Obj *gFindUnitById(int) __asm__("func_8004D664");
extern void func_80025CFC(Obj *);
extern void func_8005A190(Data *, int, int);

/**
 * @brief Retires one unit: on the first pass it spawns the destruction effect and the
 *        two cell debris objects, then either starts the in-game death sequence or,
 *        outside gameplay, empties the unit's gauge and installs the dead handlers.
 *
 * @param o Unit object; +0xAC data block, +0xDC child, +0xEC owner, +0x8C id,
 *        +0x14 tag, +0x1C/+0x1E angles, +0x20 active byte, +0x24 flag word, +0x08
 *        flag word, +0x04 update pointer, +0x134 stats pointer.
 * @return Nothing.
 *
 * @par Confirmed
 * Gate:
 * first = (data->flag == 0), and the flag byte is cleared immediately, so the effect
 * and scoring block runs once per retirement. func_8002A810(o, 0, first) is the base
 * entity preamble and gates the whole body: it latches +0x24 bit 0x01000010, and
 * everything below only runs when it returns non-zero.
 *
 * First pass:
 * func_80058C3C(o->owner, child + 0x5C, o) spawns the attached kind-7 effect,
 * func_800402DC(o, 3, 0, 1, 0) and func_800402DC(o->child, 9, 0, 1, 0) spawn cell
 * objects of columns 3 and 9, and in game mode (D_8009FA94 +0x5C == 1) the sound
 * 0x41 is played for the object. Both object and child get bit 0x800000 in their
 * +0x08 flag word, and the +0x1E angle is copied into both +0x1C angle words.
 *
 * In-game branch:
 * The data timer is 1200 on the first pass and 150 otherwise, the +0x24 flag word of
 * both parts gains 0xA5, each part is activated through func_8003BA50 when its +0x20
 * byte is set and +0x24 bit 0x40000 is clear, an empty pointer word in the data block
 * is stamped from D_8009FAA4, and the data counter at +0x16 is incremented. When the
 * object carries an id, the killer is looked up with func_8004D664(id & 0x3FFFFFFF):
 * with D_8009FA0C == 1 the local player D_8009F9EC takes the credit (its stats word
 * at +0x3C is incremented and +0x18 zeroed) when the object's tag matches the
 * player's +0x14 tag under 0x3FFF; otherwise the victim's team byte +0xC1 picks
 * D_8009FA58[team ^ 1] and that block's data counter at +0x14 is incremented when the
 * killer's tag matches it.
 *
 * Non-gameplay branch:
 * func_80025CFC(o) runs unless the screen state D_8009FA00 is 5 (it flags the team
 * entry of D_8009F65C once and plays sound 0x18), func_8005A190(data, 0, angle) writes
 * the unit's gauge to zero, and the update pointers at +0x04 become D_800578B4 for the
 * object and D_800578E4 for the child.
 *
 * @par Likely role
 * The message 4 ("retire") half of the unit class, in contrast to its message 6
 * damage/update handler func_800588D4: the explosion is spawned by the engine's
 * attached-effect and cell spawners, scoring is settled here, and the actual dead
 * behaviour lives in the D_800578B4/D_800578E4 update pair.
 *
 * @par Unknown
 * What exactly the two data counters (+0x14 and +0x16) count, what D_8009FAA4
 * stamps into the empty pointer word, and why the non-gameplay branch skips the
 * sound for screen state 5.
 *
 * @par Callers
 * func_8005908C, message 4 of the class-1 unit handler.
 */
void func_80058D78(Obj *o)
{
    int first;
    register int team __asm__("$2");
    register Obj **table __asm__("$3");
    u16 angle;
    Obj *found, *other, *child;
    Data *data;
    first = o->data->flag == 0;
    o->data->flag = 0;
    if (gRetireEntityBase(o, 0, first)) {
        if (first) {
            func_80058C3C(o->owner, (u8 *)o->child + 0x5c, o);
            gSpawnTypeAtOwner(o, 3, 0, 1, 0);
            gSpawnTypeAtOwner(o->child, 9, 0, 1, 0);
            if (gGameSession->active == 1) func_80046C84(0x41, o);
        }
        o->flags8 |= 0x800000;
        o->child->flags8 |= 0x800000;
        if (gGameSession->active == 1) {
            o->data->timer = first ? 1200 : 150;
            o->flags24 |= 0xa5;
            o->child->flags24 |= 0xa5;
            child = o->child;
            __asm__("" : "+r"(child));
            angle = o->other;
            child->angle = angle;
            o->angle = angle;
            if (o->active && !(o->flags24 & 0x40000)) gFlagQuartileChange(o, 0);
            if (o->child->active && !(o->child->flags24 & 0x40000)) gFlagQuartileChange(o->child, 0);
            if (!o->data->ptr20) o->data->ptr20 = gFrameTime;
            o->data->count16++;
            if (o->id) {
                found = gFindUnitById(o->id & 0x3fffffff);
                if (gPlayerCount == 1) {
                    if (found && gSinglePlayerEntity && gSinglePlayerEntity->tag == (found->tag & 0x3fff)) {
                        ((struct Stats *)gSinglePlayerEntity->stats)->b++;
                        ((struct Stats *)gSinglePlayerEntity->stats)->a = 0;
                    }
                } else {
                    data = o->data;
                    __asm__ volatile ("nop" : "+r"(data) : : "memory");
                    __asm__ volatile ("lbu %0,0xc1(%2)\n\tlui %1,%%hi(D_8009FA58)\n\taddiu %1,%1,%%lo(D_8009FA58)" : "=r"(team), "=r"(table) : "r"(data));
                    other = table[(s8)(team ^ 1)];
                    if (other && (found->tag & 0x3fff) == other->tag) other->data->count14++;
                }
            }
        } else {
            if (gScreenState != 5) func_80025CFC(o);
            func_8005A190(o->data, 0, o->angle);
            o->update = D_800578B4;
            o->child->update = D_800578E4;
        }
    }
}
