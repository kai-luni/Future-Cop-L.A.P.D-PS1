// Categories: Resources > Asset Loader | Graphics > Animation
#include "game/types.h"

extern s32 gVertexBlockTable __asm__("D_8009F554");
extern s32 gNormalBlockTable __asm__("D_8009F558");
extern s32 gRelocBlockTable __asm__("D_8009F55C");
extern s32 gRefBlockTable __asm__("D_8009F560");
extern s32 gAnimNodeList __asm__("D_8009F564");
extern s32 gAnimWaveList __asm__("D_8009F568");

void gInitModelBlockTables(s32 arg0) __asm__("func_80012E1C");

/**
 * @brief Splits a buffer into four model block pointer tables and clears the
 * animation lists.
 *
 * @param arg0 Base address of the table buffer.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores the base and base plus 0x400, 0x800, and 0xC00 to four successive
 * GP-relative words. Clears the next two words.
 *
 * @par Globals
 * gVertexBlockTable, gNormalBlockTable, gRelocBlockTable, and gRefBlockTable
 * receive the table bases. gAnimNodeList and gAnimWaveList are cleared.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80024A24.
 *
 * @par Role and path
 * Engine startup prepares the indexed block tables before model chunks are
 * loaded.
 *
 * @par Unknown
 * The routine places no bound on how many entries a subsequent parser writes.
 */
void gInitModelBlockTables(s32 arg0)
{
    register s32 next __asm__("$2");

    next = arg0 + 0x400;
    gNormalBlockTable = next;
    next = arg0 + 0x800;
    gRelocBlockTable = next;
    next = arg0 + 0xC00;
    gVertexBlockTable = arg0;
    gRefBlockTable = next;
    gAnimNodeList = 0;
    gAnimWaveList = 0;
}
