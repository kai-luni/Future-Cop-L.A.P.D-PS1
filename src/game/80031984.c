// Categories: Game Engine > Init | Gameplay > Map
#include "game/types.h"

struct Unk31Node;

extern struct Unk31Node *gMapZoneList __asm__("D_8009F6F8");
void gInitMapZoneList(void) __asm__("func_80031984");


/**
 * @brief Clears the map trigger/zone linked-list head D_8009F6F8 during mission boot.
 *
 * Sets D_8009F6F8 to 0 (sw $zero, 0x4E4($gp)) before zone nodes are registered for the new mission.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F6F8 - head pointer of the map trigger/zone node list.
 *
 * @par Control flow
 * Straight-line store of 0 and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F6F8 = 0.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024CDC, mission boot sequence) / Likely: none / Unknown: none.
 */
void gInitMapZoneList(void)
{
    gMapZoneList = 0;
}
