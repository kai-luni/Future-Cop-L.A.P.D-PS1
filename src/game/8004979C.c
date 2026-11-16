// Categories: Graphics > Textures | Resources > Descriptor Table
#include "game/types.h"

extern void *D_8009F77C;

/**
 * @brief Publishes the active sprite/tile descriptor table pointer into D_8009F77C.
 *
 * Writes value into D_8009F77C (sw $a0, 0x568($gp)), replacing the active descriptor table read by texture/tile lookup routines.
 *
 * @param value Pointer to the newly built descriptor table.
 *
 * @par Globals
 * D_8009F77C - active sprite/tile descriptor table pointer.
 *
 * @par Control flow
 * Straight-line store of value and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F77C = value.
 *
 * @par Callers
 * Confirmed: func_8005A2BC (0x8005A3E0, sprite/tile descriptor builder) / Likely: none / Unknown: none.
 */
void func_8004979C(void *value)
{
    D_8009F77C = value;
}
