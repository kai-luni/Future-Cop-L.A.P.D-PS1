// Categories: Resources > Loading
#include "game/types.h"

struct Resource;

struct Unk607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};

struct Unk6A898 {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct Unk607B8 sub;
    u8 unk1A[6];
};

struct Unk60838Arg {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

struct Unk6AAEC {
    u32 unk00;
    struct Unk6A898 obj1;
    struct Unk6A898 obj2;
    void *unk44[4];
};

extern s32 gDescriptorRegistryCount __asm__("D_8009F848");
extern struct Unk6AAEC gVmRecordTable[] __asm__("D_800A3340");
extern u8 gVmRecordObjects[] __asm__("D_800A3344");

extern void *func_80019C38(u32 a0, u32 a1, u32 a2, u32 a3);
extern void *func_80019CA0(u32 a0, u32 a1, u32 a2, u32 a3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern void gScaleRegistryObjectFields(struct Unk6A898 *value) __asm__("func_8006A898");
extern void gFreeBlock(struct Unk60838Arg *arg0) __asm__("func_800181D4");
extern void *memcpy(void *dest, const void *src, u32 count);

/**
 *  @brief Loads a resource-backed entry when the six-entry limit permits.
 *
 *  Copies a tagged record into the entry table and resolves four references.
 *  The input is passed to a final helper even when the table is full.
 *  @param arg0 Descriptor supplying resource lookup fields.
 *  @par Globals
 *  D_8009F848 counts entries and is incremented before loading.
 *  gVmRecordTable stores copied records; gVmRecordObjects is the object area of that table (base + 4).
 *  @par Control flow
 *  If the count is below six, loads two records and loops over four references.
 *  A zero reference name produces a null pointer; otherwise it is resolved.
 *  @par Calls
 *  func_80019C38 and func_80019CA0 obtain tagged data; memcpy copies the record.
 *  func_8006A898 processes both objects; func_80019BF0 resolves references.
 *  func_800181D4 processes arg0 unconditionally.
 *  @par Side effects
 *  Updates the entry count and table and invokes the final helper.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gRegistryLoad(struct Unk60838Arg *arg0) __asm__("func_8006A918");

void gRegistryLoad(struct Unk60838Arg *arg0)
{
    struct Unk6AAEC *entry;
    u8 *data;
    u8 *second;
    s32 i;
    s32 index;

    if (gDescriptorRegistryCount < 6) {
        index = gDescriptorRegistryCount++;

        data = func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C);

        memcpy(&gVmRecordTable[index], data + 0x10, 0x44);

        gScaleRegistryObjectFields((struct Unk6A898 *)(gVmRecordObjects + (u32)(index * 84)));
        gScaleRegistryObjectFields((struct Unk6A898 *)(gVmRecordObjects + (u32)(index * 84) + 0x20));

        second = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);
        entry = (struct Unk6AAEC *)(gVmRecordObjects + (u32)(index * 84) - 4);
        __asm__("" : : "r"(entry));

        {
            register s32 *sp __asm__("$17");
            sp = (s32 *)(second + 8);
            for (i = 0; i < 4; i++, sp += 2) {
                if (sp[2] != 0) {
                    entry->unk44[i] = func_80019BF0(sp[1], sp[2]);
                } else {
                    entry->unk44[i] = 0;
                }
            }
        }
    }

    gFreeBlock(arg0);
}
