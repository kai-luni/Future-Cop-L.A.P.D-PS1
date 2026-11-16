// Categories: Resources > Resource State | Gameplay > Entity Update
#include "../../include/game/types.h"

extern void func_80019A74(s32 *value);
extern s32 D_8009F9F4;

/**
 * @brief Marks a resource as released and latches its pointer.
 *
 * func_80019A74 runs the resource release bookkeeping and the pointer is then
 * stored in D_8009F9F4, the slot the spawn path later reports when the
 * resource count runs out.
 * @par Globals
 * D_8009F9F4 - last released resource pointer.
 * @par Control flow
 * None.
 * @par Calls
 * func_80019A74 - resource release.
 * @par Side effects
 * Updates the resource state and the global pointer.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
void func_8004D924(s32 *value)
{
    func_80019A74(value);
    D_8009F9F4 = *value;
}
