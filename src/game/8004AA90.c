// Categories: Graphics > Effects > Object Pool | Resources > Memory Management
#include "game/types.h"

struct Unk4AA90 {
    u8 unk00[0x48];
    struct Unk4AA90 *next;
};

extern struct Unk4AA90 *D_8009F798;

/**
 * @brief Pops the head of the fragment free list D_8009F798, keeping the
 *        last node as a sentinel.
 *
 * @return The popped node, or 0 when only the sentinel remains.
 *
 * @par Confirmed
 * current = D_8009F798; when current->next == 0 the list is treated as
 * empty (returns 0, head unchanged); otherwise the head advances to
 * current->next and current is returned.
 *
 * @par Likely role
 * The fragment allocator of the polygon fracture func_8004B5F8: the
 * sentinel convention keeps one node permanently chained so the list
 * pointer is never null.
 *
 * @par Callers
 * func_8004B5F8.
 */
struct Unk4AA90 *func_8004AA90(void)
{
    struct Unk4AA90 *current;

    current = D_8009F798;
    if (current->next == 0) {
        goto empty;
    }
    D_8009F798 = current->next;
    goto done;
empty:
    current = 0;
done:
    return current;
}
