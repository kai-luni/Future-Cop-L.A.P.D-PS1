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
extern void gSelectViewRecord(s32) __asm__("func_80017B60");
extern void gGteLoadRotation(void *) __asm__("func_80010820");
extern void gDrawRegionTiles(s32, s32, void *) __asm__("func_8006531C");
extern void gDrawCellWindowObjects(s32, s32, void *) __asm__("func_8004A924");
extern void gDrawCellWindowModels(s32, s32, void *) __asm__("func_80084398");
extern void gRunRegionObjectPass(s32) __asm__("func_80062EB8");
extern void gRunViewScenePasses(s32) __asm__("func_8003F6E4");
void gDrawViewLayers(s32 index, struct Object66E98 *object) __asm__("func_80066E98");


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
 *
 * @par Environment path
 * The environment pass of one view: selects the view record, loads the camera
 * rotation, then draws the same region in three layers (tiles, cell objects,
 * cell models) and closes it. It draws what the view sees, independent of the
 * actor lists, so it is the piece a minimal scene needs.
 */
void gDrawViewLayers(s32 index, struct Object66E98 *object)
{
    struct Region66E98 *region;
    s32 x;
    s32 y;

    gSelectViewRecord(object->info->id);
    gGteLoadRotation(gCamera + 0x28);
    region = &gCoverageGrid[index];
    x = gGridOriginX[index].x;
    y = gGridOriginX[index].y;
    gDrawRegionTiles(x, y, region);
    gGteLoadRotation(gCamera + 0x28);
    gDrawCellWindowObjects(x, y, region);
    gDrawCellWindowModels(x, y, region);
    gRunRegionObjectPass(index);
    gRunViewScenePasses(index);
}
