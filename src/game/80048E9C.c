// Categories: Graphics > Text > Font Upload | Resources > Asset Loading
#include "game/types.h"

extern void func_80048A14(s32 value, s32 *source);

/**
 * @brief Registers a font from an asset table entry: forwards the entry's
 *        first word and the entry itself to func_80048A14.
 *
 * @param source Asset entry; *source is the font id/format word.
 * @return Nothing.
 *
 * @par Confirmed
 * Single call func_80048A14(*source, source); no other code.
 *
 * @par Likely role
 * The font case of the asset loader dispatcher func_80024EC8: each loaded
 * font asset is handed to the registration helper with its descriptor.
 *
 * @par Callers
 * func_80024EC8.
 */
void func_80048E9C(s32 *source)
{
    func_80048A14(*source, source);
}
