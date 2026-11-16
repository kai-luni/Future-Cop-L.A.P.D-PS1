// Categories: Graphics > Effects > Object Pool | Utilities > Memory Management
#include "game/types.h"

struct Unk6CA68 {
    void *first;
    void *second;
};

extern void func_800181D4(void *block);

/**
 * @brief Frees a two-pointer mesh/effect wrapper and both of its sub-allocations.
 *
 * Releases object->first (+0x00), object->second (+0x04), and finally object itself via func_800181D4 (e.g. a Mesh struct allocated by func_8006C84C).
 *
 * @param object Pointer to a struct whose first two words (+0x00, +0x04) hold allocated buffer pointers.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Unconditional straight-line sequence of three func_800181D4 calls.
 *
 * @par Calls
 * func_800181D4 - called for object->first, object->second, and object.
 *
 * @par Side effects
 * Returns three heap/pool allocations to the memory manager; does not null pointers prior to freeing object.
 *
 * @par Callers
 * Confirmed: func_8005E074 (0x8005E088, effect teardown) and func_8006E814 (0x8006E924, message 10 pooled effect mesh release).
 */
void func_8006CA68(struct Unk6CA68 *object)
{
    func_800181D4(object->first);
    func_800181D4(object->second);
    func_800181D4(object);
}
