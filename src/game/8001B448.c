// Categories: Resources > Memory Management
#include "game/types.h"

extern void *D_8009F5C8;
extern void *D_8009F5CC;
extern void *D_8009F5D0;
extern void *D_8009F5D4;
extern void *D_8009F5D8;
extern void *D_8009F5DC;

extern void func_800181D4(void *block);

/**
 * @brief Releases the render arena allocated by func_8001B2C8().
 *
 * The six blocks are freed in allocation order through func_800181D4(), the
 * shared deallocator: the two 52-byte-record pools, the two 1024-entry
 * 28-byte arrays and the two word tables. The globals keep their (now stale)
 * pointers; nothing is cleared here, so a later free would be a double free -
 * the callers only run this once per module teardown.
 *
 * @return Nothing.
 *
 * @par Globals written
 * Only through the deallocator; D_8009F5C8..D_8009F5DC themselves are not
 * reset.
 *
 * @par Callers
 * func_800249BC, func_80024D14 (module teardown paths) and func_80076A2C
 * (after the intro has been shown, before the front-end loop starts).
 */
void func_8001B448(void)
{
    func_800181D4(D_8009F5C8);
    func_800181D4(D_8009F5CC);
    func_800181D4(D_8009F5D0);
    func_800181D4(D_8009F5D4);
    func_800181D4(D_8009F5D8);
    func_800181D4(D_8009F5DC);
}
