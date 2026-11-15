// Categories: Graphics > Model Renderer > Vertex Deform | Graphics > Animation
#include "game/types.h"

struct Unk6CAAC {
    s16 x;
    s16 y;
    s16 z;
    s8 i0;
    s8 i1;
};

struct Out6CAAC {
    s16 a;
    s16 b;
    s16 c;
    s16 pad;
};

/**
 * @brief Applies a per-vertex scale table to a vertex stream (skinning/scale
 *        pass).
 *
 * @param dst Destination (three s16 plus padding per vertex).
 * @param tbl Scale table, four s16 per vertex; the entries at index
 *            src->i0 and src->i1 (and index 1) are used.
 * @param count Vertex count.
 * @param src Source vertices (x, y, z plus two selector bytes).
 * @param scale Unused by the loop - the aliased pointer arithmetic uses it, but
 *              the values come from the table.
 * @return Nothing.
 *
 * @par Confirmed
 * Per vertex: a = (x * tbl[i0]) >> 12, b = (y * tbl[1]) >> 12 and
 * c = (z * tbl[i1]) >> 12, all 16-bit; the table advances by four half-words and
 * the destination by one vertex.
 *
 * @par Likely role
 * The scale step of the model animator: each vertex carries two indices into a
 * small table (per-bone or per-keyframe scales), and 12-bit fixed point is the
 * engine's standard for these factors.
 *
 * @par Callers
 * func_8006CB58 (the model draw entry).
 */
void func_8006CAAC(struct Out6CAAC *dst, s16 *tbl, s16 count, struct Unk6CAAC *src, s16 scale)
{
    s32 n = count;
    register s32 i __asm__("$8") = 0;
    register s32 r __asm__("$2") = scale;
    s32 off;
    register s32 addr __asm__("$2");

    __asm__ volatile ("" : "+r"(i), "+r"(r));
    if (n > 0) {
        off = r * n;
        addr = off << 3;
        __asm__ volatile ("addu %0,%1,%2" : "=r"(dst) : "r"(addr), "r"(dst));
        do {
            dst->a = (src->x * tbl[src->i0]) >> 12;
            dst->b = (src->y * tbl[1]) >> 12;
            dst->c = (src->z * tbl[src->i1]) >> 12;
            i++;
            tbl += 4;
            dst++;
        } while (i < n);
    }
}
