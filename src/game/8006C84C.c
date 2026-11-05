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

extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800181D4(void *ptr);
extern void func_8006C4C4(void *vertices, s16 step, s16 flags);
extern void func_8006C768(void *faces, s32 rows, s32 cols, s32 count);
extern s32 D_8009FA08;
extern s32 D_8009FA1C;

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
    mesh = func_80017EBC(0x1c, 1);
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
    mesh->vertices = func_80017EBC(mesh->vertex_count * 8, 1);
    if (mesh->vertices == 0) {
        func_800181D4(mesh);
        return 0;
    }
    mesh->faces = func_80017EBC(mesh->face_count * 5, 1);
    if (mesh->faces == 0) {
        func_800181D4(mesh->vertices);
        func_800181D4(mesh);
        return 0;
    }
    mesh->flags = flags;
    if (flags & 1) {
        mesh->width = mesh->width >> 1;
    }
    mesh->unk10 = D_8009FA08;
    mesh->unk14 = D_8009FA1C;
    func_8006C4C4(mesh->vertices, step, flags & 0x10);
    func_8006C768(mesh->faces, rows, cols, mesh->vertex_count);
    return mesh;
}
