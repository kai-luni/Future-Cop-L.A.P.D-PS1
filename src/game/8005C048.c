// Categories: Gameplay > Map > Occupancy | Gameplay > Weapons
#include "game/types.h"

extern int gWeaponFireDispatch();

struct Unk5C048Def {
    u8 unk00[4];
    u8 kind;
    u8 unk05[0x29];
    u16 value;
};

struct Unk5C048Result {
    u8 unk00[0x20];
    struct Unk5C048Def *def;
};

struct Unk5C048Event {
    u16 value;
    u8 unk02[0x0D];
    u8 column;
};

extern s32 gGetEntitySpawnBank(void *object) __asm__("func_8003BF50");
extern struct Unk5C048Result *gSpawnTypeAtOwner(void *owner, u8 column, s32 arg2, s32 row,
                                            s32 arg4) __asm__("func_800402DC");

/**
 * @brief Queries the map cell under an event and, when it holds a kind-0x5C
 *        object, writes the event value into that object's definition.
 *
 * @param object Query owner.
 * @param event Event record; +0 the value, +0xF the column.
 * @return Nothing.
 *
 * @par Confirmed
 * result = func_800402DC(object, event->column, 0, func_8003BF50(object),
 * 0); a non-null result whose def->kind == 0x5C receives def->value =
 * event->value.
 *
 * @par Likely role
 * The weapon-fire interaction of gWeaponFireDispatch with special map objects
 * (kind 0x5C): likely a switch/target that stores the last trigger value,
 * e.g. a destructible panel or a scoring trigger.
 *
 * @par Unknown
 * What object kind 0x5C represents.
 *
 * @par Callers
 * gWeaponFireDispatch.
 */
void gWriteCellEventValue(void *object, struct Unk5C048Event *event) __asm__("func_8005C048");

void gWriteCellEventValue(void *object, struct Unk5C048Event *event)
{
    struct Unk5C048Result *result;

    result = gSpawnTypeAtOwner(object, event->column, 0, gGetEntitySpawnBank(object), 0);
    if (result != 0 && result->def->kind == 0x5C) {
        result->def->value = event->value;
    }
}
