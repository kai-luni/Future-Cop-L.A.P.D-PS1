// Categories: Graphics > Region Drawing | Graphics > Scene Graph
#include "game/types.h"

struct Info66E98 {
    u8 pad[0xAC];
    s8 id;
};
struct Object66E98 {
    u8 pad[0xAC];
    struct Info66E98 *info;
};
struct Region66E98 {
    u8 data[400];
};
struct Coord66E98 {
    u8 x;
    u8 y;
};
extern u8 *gCamera __asm__("D_8009F9E4");
extern struct Region66E98 gCoverageGrid[] __asm__("D_800AB3B8");
extern struct Coord66E98 gGridOriginX[] __asm__("D_8009F834");
extern void func_80017B60(s32);
extern void func_80010820(void *);
extern void func_8006531C(s32, s32, void *);
extern void func_8004A924(s32, s32, void *);
extern void func_80084398(s32, s32, void *);
extern void func_80062EB8(s32);
extern void func_8003F6E4(s32);

/**
 * @brief Draws the three region layers of one object and commits the pass.
 *
 * @param index Frame/region index, also the argument of the two finishing calls.
 * @param object Object: info block at +0xAC, whose id byte at +0xF8 selects the
 *        region.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80017B60(object->info->id) prepares the id, func_80010820(D_8009F9E4 +
 * 0x28) loads the camera matrix, then the same (x, y, region) triple goes to
 * func_8006531C, func_8004A924 and func_80084398 - three independent drawers on
 * the 400-byte region block D_800AB3B8. Finally func_80062EB8(index) and
 * func_8003F6E4(index) close the frame.
 *
 * @par Likely role
 * The per-object region pass: three layers (background, actors, foreground) of
 * the same screen region, drawn through the camera.
 */
void func_80066E98(s32 index, struct Object66E98 *object)
{
    struct Region66E98 *region;
    s32 x;
    s32 y;

    func_80017B60(object->info->id);
    func_80010820(gCamera + 0x28);
    region = &gCoverageGrid[index];
    x = gGridOriginX[index].x;
    y = gGridOriginX[index].y;
    func_8006531C(x, y, region);
    func_80010820(gCamera + 0x28);
    func_8004A924(x, y, region);
    func_80084398(x, y, region);
    func_80062EB8(index);
    func_8003F6E4(index);
}
