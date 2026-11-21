// Categories: Resources > Entity Loading | Game Engine > Object Creation
#include "game/types.h"

struct Resource;

struct Unk4D6D4 {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    u8 unk0C[0x10];
    s32 unk1C;
    s32 unk20;
};

struct Unk4D6D4Data {
    u8 unk00[4];
    u8 type;
};

struct Unk4D6D4Entry {
    s32 group;
    s32 name;
};

struct Unk4D6D4List {
    u32 count;
    struct Unk4D6D4Entry entries[1];
};

struct Unk4D6D4LSRa {
    s32 unk00;
    u32 size;
    u32 count;
};

struct EntityDef {
    s32 unk00;
    void *unk04;
    void *unk08;
    void *unk0C;
};

extern u8 gMessageDoneFlag __asm__("D_8009F7AC");

extern void gBuildCoarseHeightGrid(void) __asm__("func_800695A8");
extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 tag, s32 arg3);
extern void *func_80019CA0(s32 arg0, s32 arg1, u32 tag, s32 arg3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern s32 func_8002AE54(u8 type);
extern s32 func_8004D4D4(struct EntityDef *def);
extern void gFreeBlock(void *block) __asm__("func_800181D4");

extern void gLoadMapZone(struct Unk4D6D4 *arg0) __asm__("func_800319D4");
extern void func_80084110(struct Unk4D6D4 *arg0);
extern void gLoadActorLightNode(struct Unk4D6D4 *arg0) __asm__("func_80049EA8");
extern void gLoadAttackDefinition(struct Unk4D6D4 *arg0) __asm__("func_80060838");
extern void gRegistryLoad(struct Unk4D6D4 *arg0) __asm__("func_8006A918");
extern void gLoadActorDef5E(struct Unk4D6D4 *arg0) __asm__("func_8004488C");
extern void gLoadActorDef5D(struct Unk4D6D4 *arg0) __asm__("func_80041070");
extern void gLoadActorDef5C(struct Unk4D6D4 *arg0) __asm__("func_8006ED78");
extern void gLoadActorDef5A(struct Unk4D6D4 *arg0) __asm__("func_8004CA98");
extern void func_8003BDA0(struct Unk4D6D4 *arg0);
extern void gLoadActorDef59(struct Unk4D6D4 *arg0) __asm__("func_800706C4");
extern void func_800740B8(struct Unk4D6D4 *arg0);
extern void func_80076878(struct Unk4D6D4 *arg0);
void gLoadActorChunk(struct Unk4D6D4 *arg0) __asm__("func_8004D6D4");


/**
 *  @brief Loads an entity definition and dispatches initialization by its resource type.
 *
 *  Recognized types use dedicated handlers; the default path resolves a resource list and creates an entity.
 *  A pending global flag triggers preliminary processing before the resource lookup.
 *  @param arg0 Entity loading record containing resource identifiers and output fields.
 *  @par Globals
 *  D_8009F7AC is cleared when set before preliminary processing.
 *  @par Control flow
 *  Dispatches thirteen recognized types; otherwise loops over list entries and resolves nonzero names.
 *  @par Calls
 *  func_800695A8 handles the flag; func_80019C38 and func_80019CA0 retrieve chunks; func_80019BF0 resolves names.
 *  func_8002AE54 maps the type, func_8004D4D4 creates the entity, and func_800181D4 processes the record.
 *  Type-specific handlers are called directly for recognized types.
 *  @par Side effects
 *  Clears the flag, rewrites the resource list and record fields, and invokes initialization handlers.
 *  @par Callers
 *  Confirmed: func_80024EC8.
 */
void gLoadActorChunk(struct Unk4D6D4 *arg0)
{
    struct EntityDef def;
    struct EntityDef *defp;
    struct Unk4D6D4LSRa *lsra;
    u32 i;
    s32 type_value;
    void *list;

    if (gMessageDoneFlag != 0) {
        gMessageDoneFlag = 0;
        gBuildCoarseHeightGrid();
    }

    defp = &def;
    defp->unk08 = (void *)(func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C) - 4);

    switch (((struct Unk4D6D4Data *)defp->unk08)->type) {
    case 0x5F:
        gLoadMapZone(arg0);
        break;
    case 0x60:
        func_80084110(arg0);
        break;
    case 0x61:
        gLoadActorLightNode(arg0);
        break;
    case 0x62:
        gLoadAttackDefinition(arg0);
        break;
    case 0x63:
        gRegistryLoad(arg0);
        break;
    case 0x5E:
        gLoadActorDef5E(arg0);
        break;
    case 0x5D:
        gLoadActorDef5D(arg0);
        break;
    case 0x5C:
        gLoadActorDef5C(arg0);
        break;
    case 0x5A:
        gLoadActorDef5A(arg0);
        break;
    case 0x5B:
        func_8003BDA0(arg0);
        break;
    case 0x59:
        gLoadActorDef59(arg0);
        break;
    case 0x58:
        func_800740B8(arg0);
        break;
    case 0x57:
        func_80076878(arg0);
        break;
    default:
        lsra = (struct Unk4D6D4LSRa *)func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);
        defp->unk04 = &lsra->count;
        lsra->count = (lsra->size - 0xC) >> 3;
        for (i = 0; i < ((struct Unk4D6D4List *)defp->unk04)->count; i++) {
            if (((struct Unk4D6D4List *)defp->unk04)->entries[i].name != 0) {
                ((struct Unk4D6D4List *)defp->unk04)->entries[i].name =
                    (s32)func_80019BF0(
                        ((struct Unk4D6D4List *)defp->unk04)->entries[i].group,
                        ((struct Unk4D6D4List *)defp->unk04)->entries[i].name);
            }
        }
        type_value = func_8002AE54(((struct Unk4D6D4Data *)defp->unk08)->type);
        list = defp->unk04;
        defp->unk00 = type_value;
        defp->unk0C = arg0;
        arg0->unk08 = 0;
        arg0->unk00 = (s32)list;
        arg0->unk04 = func_8004D4D4(defp);
        gFreeBlock(arg0);
        break;
    }
}
