// Categories: Resources > Asset Loading | Gameplay > Object Spawning
#include "game/types.h"

struct Unk84110 {
    u8 *unk00;
    u8 unk04[0x18];
    s32 unk1C;
    u32 unk20;
};

struct Resource;

extern u32 *func_80019CA0(u8 *arg0, u32 arg1, u32 arg2, s32 arg3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern u8 *func_80019C38(u8 *arg0, u32 arg1, u32 arg2, s32 arg3);
extern void gSpawnSceneNode(u8 *arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80083A10");
extern void gFreeBlock(struct Unk84110 *arg0) __asm__("func_800181D4");

/**
 * @brief Loads a spawn-resource bundle: resolves its entry count, fetches the
 *        resource node and registers the actor table, then frees the request.
 *
 * @param arg0 Request block; +0 the data pointer, +0x1C a group id, +0x20 the
 *        data size.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80019CA0(data, size, 0x6152534C, group) locates the 'LSRa' chunk;
 * data[2] = (data[1] - 0xC) >> 3 becomes the entry count. func_80019BF0
 * (data[1], data[2]) loads the resource node and its pointer is stored back
 * into data[2]. func_80083A10 then binds the 'TCAt' chunk
 * (func_80019C38(..., 0x74414354, ...) - 4) with node[0], -1, 0, and
 * func_800181D4 frees the request block.
 *
 * @par Likely role
 * The deferred spawn-asset loader: the request spawner func_8004D6D4 queues
 * these blocks so actor tables ('TCAt') and their resource lists ('LSRa')
 * are bound before the objects appear.
 *
 * @par Unknown
 * The exact chunk header layout and what node[0] selects in func_80083A10.
 *
 * @par Callers
 * func_8004D6D4 (verified in source; the call-graph edge is missing).
 */
void func_80084110(struct Unk84110 *arg0)
{
    u32 *data;
    s32 *node;

    data = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);
    data[2] = (data[1] - 0xC) >> 3;
    data += 2;
    node = (s32 *)func_80019BF0(data[1], data[2]);
    data[2] = (u32)node;
    gSpawnSceneNode(func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C) - 4,
                  node[0], -1, 0);
    gFreeBlock(arg0);
}
