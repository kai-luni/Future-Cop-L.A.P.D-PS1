// Categories: Resources > Asset Loader | Resources > File Access
#include "game/types.h"

/**
 * @brief Searches a chunked blob for a record matching tag and section ID, returning its header pointer.
 *
 * Walks variable-length chunks from arg0 up to arg1 bytes (tag at +0x00, byte size at +0x04, section ID at +0x08). Unlike func_80019C38, returns the chunk start rather than payload +0x0C, and does not special-case arg3 == 0. On miss, returns the last visited chunk (or 0 if arg1 == 0).
 *
 * @param arg0 Base pointer of chunked data.
 * @param arg1 Blob size in bytes.
 * @param arg2 Four-byte tag compared with +0x00.
 * @param arg3 Section ID compared with +0x08.
 * @return Pointer to matching chunk header (or last chunk visited on miss; 0 if arg1 == 0).
 * @par Globals
 * None.
 * @par Control flow
 * Loops while !found && (u32)(p - arg0) < arg1; sets result = p, tests *(s32*)(p+8)==arg3 && *(u32*)p==arg2, and advances p = result + *(s32*)(result+4) on mismatch.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Confirmed: func_80049420 (0x8004944C), func_80060838 (0x80060984), func_80060E74 (0x80060EC8, 0x80060F58), func_80084110 (0x80084138), func_8004D6D4 (0x8004D83C), func_8006A918 (0x8006AA74).
 */
u8 *func_80019CA0(u8 *arg0, u32 arg1, u32 arg2, s32 arg3)
{
    u8 *p;
    u8 *result;
    s32 found;
    u8 byte_found;

    result = 0;
    p = arg0;
    found = 0;
    while (!found && (u32)(p - arg0) < arg1) {
        result = p;
        byte_found = (*(s32 *)(p + 8) == arg3) && (*(u32 *)result == arg2);
        found = byte_found;
        if (!byte_found) {
            p = result + *(s32 *)(result + 4);
        }
    }
    return result;
}
