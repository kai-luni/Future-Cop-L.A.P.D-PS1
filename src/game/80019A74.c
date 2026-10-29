#include "game/types.h"

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
};

extern struct Resource *D_8009F5B8;
extern struct Resource *D_8009F5BC;

/* Appends a resource to the global doubly linked list.
 * D_8009F5B8 is the first node and D_8009F5BC the last; a new entry is linked
 * behind the tail and becomes the new tail, or becomes head and tail when the
 * list is still empty. Registered nodes are what func_80019BF0 searches by
 * (group, name). */
void func_80019A74(struct Resource *resource)
{
    resource->next = 0;
    resource->prev = 0;
    if (D_8009F5BC != 0) {
        resource->prev = D_8009F5BC;
        D_8009F5BC->next = resource;
        D_8009F5BC = resource;
    } else {
        D_8009F5BC = resource;
        D_8009F5B8 = resource;
    }
}
