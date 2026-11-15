// Categories: Graphics > Lighting > Scene Node | Resources > Memory Management
#include "game/types.h"

struct Unk75478 {
    u8 unk00[0x24];
    void *light;
};

extern void func_800498F0(void *object);

/**
 * @brief Releases the light object attached to a scene node.
 *
 * @param object Node; the light pointer sits at +0x24.
 * @return Nothing. The pointer is cleared, so a second call does nothing.
 *
 * @par Confirmed
 * if (object->light != 0) { func_800498F0(object->light); object->light = 0; }
 *
 * @par Callers
 * func_80075A84 and the unit at 0x80075698 - the node teardown of the polygon
 * renderer.
 */
void func_80075478(struct Unk75478 *object)
{
    if (object->light != 0) {
        func_800498F0(object->light);
        object->light = 0;
    }
}
