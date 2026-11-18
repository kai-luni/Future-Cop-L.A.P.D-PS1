// Categories: Graphics > Mesh Generation | Graphics > Effects
#include "game/types.h"

typedef struct {
    void *vertices;
    void *faces;
    s16 face_count;
    s16 vertex_count;
    s8 width;
    s8 height;
    u8 pad[2];
    s32 unk10;
    s32 unk14;
    u8 flags;
    u8 pad19[3];
} Mesh;

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gFreeBlock(void *ptr) __asm__("func_800181D4");
extern void func_8006C4C4(void *vertices, s16 step, s16 flags);
extern void func_8006C768(void *faces, s32 rows, s32 cols, s32 count);
extern s32 gTableBuffer __asm__("D_8009FA08");
extern s32 gLevelVertexBlock __asm__("D_8009FA1C");

/**
 * @brief Allocates and fills a dome-patch mesh for the effect objects.
 *
 * Quantizes the requested step to a divisor of 0x200 and builds a Mesh header
 * with 8-byte vertices and 5-byte faces. The vertex fill sweeps a spherical
 * octant (rows pole-to-equator over angles 0..0x400, columns a 0x200 wedge);
 * the radius is fixed 0x1000, or grows 0x800 to 0x1000 with flags bit 0x10.
 * Faces are a pole triangle fan plus quad strips with byte vertex indices.
 *
 * @param arg0 Requested angular step, 1..0x200, quantized to divide 0x200.
 * @param flags Bit 0 halves the stored width; bit 0x10 selects the expanding
 *        radius variant and is passed on to the vertex fill.
 * @return The mesh, or 0 when any allocation fails.
 *
 * @par Globals
 * D_8009FA08 - shared 0x1B18-byte graphics buffer; pointer copied to +0x10.
 * D_8009FA1C - shared 0x400-byte graphics buffer; pointer copied to +0x14.
 *
 * @par Control flow
 * Step quantization, then three allocation stages (header, vertices, faces)
 * with rollback frees on failure. Width is halved for flags bit 0 before the
 * two fill calls run.
 *
 * @par Calls
 * func_80017EBC - pool allocation with tag 1 for header and both buffers.
 * func_800181D4 - frees already-allocated parts when a later stage fails.
 * func_8006C4C4 - writes the spherical vertex grid.
 * func_8006C768 - writes the fan and strip faces.
 *
 * @par Side effects
 * Allocates pool memory; writes the mesh header, vertex and face buffers.
 *
 * @par Callers
 * Confirmed: func_8005D3FC (effect emitter blocks, steps 0x100 and 0x200),
 * func_8006E814 (mesh effect init, step 0x100).
 */
Mesh *func_8006C84C(s16 arg0, s32 flags)
{
    Mesh *mesh;
    s32 step;
    s32 count;
    register s32 rows __asm__("$20");
    register s32 cols __asm__("$22");
    register s32 stride __asm__("$17");

    if ((u16)(arg0 - 1) >= 0x200) {
        step = 0x200;
    } else {
        step = 0x200 / (0x200 / arg0);
    }
    count = 0x200 / step;
    mesh = gAllocMemory(0x1c, 1);
    rows = count + 1;
    stride = rows * 2;
    cols = stride - 1;
    if (mesh == 0) {
        return 0;
    }
    {
    register s32 width __asm__("$3");
    register s32 tmp __asm__("$2");
    register s32 product __asm__("$4");

    __asm__("addiu %0,%1,-2" : "=r"(width) : "r"(stride));
    __asm__("addiu %0,%1,-3" : "=r"(tmp) : "r"(stride));
    product = count * tmp;
    mesh->width = width;
    mesh->height = count;
    tmp = rows * width + 1;
    mesh->vertex_count = tmp;
    tmp = product - 1;
    mesh->face_count = rows + tmp;
    }
    mesh->vertices = gAllocMemory(mesh->vertex_count * 8, 1);
    if (mesh->vertices == 0) {
        gFreeBlock(mesh);
        return 0;
    }
    mesh->faces = gAllocMemory(mesh->face_count * 5, 1);
    if (mesh->faces == 0) {
        gFreeBlock(mesh->vertices);
        gFreeBlock(mesh);
        return 0;
    }
    mesh->flags = flags;
    if (flags & 1) {
        mesh->width = mesh->width >> 1;
    }
    mesh->unk10 = gTableBuffer;
    mesh->unk14 = gLevelVertexBlock;
    func_8006C4C4(mesh->vertices, step, flags & 0x10);
    func_8006C768(mesh->faces, rows, cols, mesh->vertex_count);
    return mesh;
}
