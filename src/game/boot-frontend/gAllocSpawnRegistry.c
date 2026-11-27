// Categories: Game Engine > Object Registry | Utilities > Memory Management
#include "game/types.h"

struct Unk60E28Table {
    s32 count;
    void *entries[0x80];
};

extern struct Unk60E28Table *gRegistryTable __asm__("D_8009F800");
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
void gAllocSpawnRegistry(void) __asm__("func_80060E28");


/**
 * @brief Allocates the 0x80-slot registry table behind D_8009F800.
 *
 * The block is 0x204 bytes: a s32 count followed by 0x80 entry pointers. Entries are added by id, removed by id through func_8006158C and the table is released by func_80060E50; the fixed slot count shows this is the live-object registry of a mission, not a growable list.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F800 - receives the new table pointer; the count word at +0 is zeroed.
 *
 * @par Control flow
 * Allocate 0x204 bytes from pool tag 2, store the pointer, clear the count, return. The allocation result is not checked.
 *
 * @par Calls
 * func_80017EBC - heap allocation from pool tag 2.
 *
 * @par Side effects
 * Consumes 0x204 bytes of the tag-2 pool; a previous table is leaked, so this has to run once per mission.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024C3C), the mission init.
 */
void gAllocSpawnRegistry(void)
{
    struct Unk60E28Table *table;

    table = gAllocMemory(0x204, 2);
    gRegistryTable = table;
    table->count = 0;
}
