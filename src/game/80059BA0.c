// Categories: Resources > File Access | Platform > CD-ROM
#include "game/types.h"

extern s32 gFileSearchGroup __asm__("D_8009F238");

extern s32 func_8001E110(s32 name, s32 arg1, void **data, s32 *size, s32 arg4);
extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Loads a whole file and returns its buffer, releasing the partial buffer
 *        when the read does not succeed.
 *
 * Thin wrapper around func_8001E110 with the lookup parameter fixed to
 * D_8009F238. The loader returns the buffer pointer and the size through two
 * out-parameters; only a positive return value - the number of bytes read - is
 * treated as success here, and every other outcome frees the buffer the loader
 * may already have allocated.
 *
 * @param name File name (or table index) understood by func_8001E110.
 * @return The file contents, or 0 when the read failed or the file was empty.
 *
 * @par Globals
 * D_8009F238 - file-layer lookup parameter, forwarded as func_8001E110's second
 * argument (zero in the retail image).
 *
 * @par Control flow
 * One call, a sign test, and on the failure path a null test before the free;
 * the success path returns the buffer unchanged.
 *
 * @par Calls
 * func_8001E110 - resolves the name, allocates the buffer and reads the file.
 * func_800181D4 - releases the buffer left behind on failure.
 *
 * @par Side effects
 * Allocates a heap block on success, frees one on failure.
 *
 * @par Callers
 * Confirmed: func_80059BFC.
 */
void *func_80059BA0(s32 name)
{
    void *data;
    void *result;
    s32 size;

    if (func_8001E110(name, gFileSearchGroup, &data, &size, 0) > 0) {
        goto success;
    }
    if (data != 0) {
        gFreeBlock(data);
    }
    result = 0;
    goto done;
success:
    result = data;
done:
    return result;
}
