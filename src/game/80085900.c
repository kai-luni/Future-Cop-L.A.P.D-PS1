// Categories: Game Engine > State Access
#include "game/types.h"

typedef struct {
    u8 pad[0x11C];
    s32 unk11C;
} Unk8009F4BC;

extern Unk8009F4BC *gMovieSliceContext __asm__("D_8009F4BC");

/**
 *  @brief Writes a signed value to a global object's field.
 *
 *  Stores the supplied value through a volatile access at offset 0x11C
 *  in the object referenced by the global pointer.
 *  @param arg0 Signed value to store.
 *  @par Globals
 *  gMovieSliceContext supplies the object pointer.
 *  @par Control flow
 *  Performs the store unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the object's field at offset 0x11C; the pointer must be valid.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80085900(s32 arg0)
{
    *(volatile s32 *)&gMovieSliceContext->unk11C = arg0;
}
