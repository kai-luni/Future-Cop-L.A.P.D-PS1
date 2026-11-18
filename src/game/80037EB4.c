// Categories: Graphics > Model Renderer > Basis Orientation | Graphics > GTE
#include "game/types.h"
struct Vec { s32 x,y,z; };
struct Model { u8 pad[0x34]; u8 *data; };
struct Self {
 u8 pad0[0x28]; struct Model *model;
 struct Vec basis[3]; struct Vec pos;
 s32 unk5c[5]; s16 unk70; u8 pad72[0x3e];
 s32 unkB0; s16 unkB4; u8 padB6[0x62];
 u32 flags; u8 pad11c[0xe]; s16 unk12a; u8 pad12c[4];
 s16 unk130; u8 pad132[4]; s16 unk136; u8 pad138[8];
 struct Vec unk140;
};
extern s32 gGetGroundHeight(struct Vec *, struct Vec *, s32, s32) __asm__("func_80066F78");
extern void func_800192AC(struct Vec *, struct Vec *, struct Vec *);
extern void gBuildRotationMatrix(s32,s32,s32,struct Vec *) __asm__("func_800188D0");
extern void gGteLoadMatrix(struct Vec *) __asm__("func_80010750");
void gOrientEntityBasis(struct Self *arg) __asm__("func_80037EB4");

extern void gGteTransformVerticesWide(u8 *, void *, s32) __asm__("func_80010234");
/**
 * @brief Orients an object's model basis (ground-normal or fixed) and fetches its
 *        frame vertex.
 *
 * @param arg Object: position, three basis vectors, the 0x140 vector, flags at
 *        +0x12A? (see the structs at the top of the file), matrix at +0x2C and
 *        output at +0x5C.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80066F78(&self->pos, 0, 0, 0) queries the ground/height, a second call with
 * &tmp gets the normal; func_800192AC(&tmp, &self->unk140, self->basis) builds the
 * basis from both, func_800188D0(self->unk12a, c, d, basis) adds the rotation, and
 * func_80010750 + func_80010234 fetch one vertex into self->unk5c. The flag bits
 * 0x200 and 0x100 and the `flags >> 30` branch choose between the ground-aligned,
 * the fixed and the two-parameter variant (c/d from +0xB0/+0xB4).
 *
 * @par Likely role
 * The "stand on the terrain" orientation of a ground object: the model's up axis
 * follows the surface normal unless the flags pin it.
 *
 * @par Environment path
 * Orients an entity's model basis according to flags 0x200 and 0x100. With
 * both set it snaps the position to the ground with func_80066F78 and writes
 * a fixed yaw basis from the direction at +0x140; with only 0x200 it blends
 * the ground normal from func_80066F78 into the up axis (1:3) and rebuilds
 * the basis with func_800192AC; with only 0x100 it writes the fixed yaw
 * basis. Otherwise it builds the matrix from the angles with func_800188D0
 * (the +0xB0/+0xB4 pair when the top flag bits are set), loads it and
 * transforms one frame vertex. Callers are func_8002B784, func_800378BC,
 * func_8003802C and func_8005027C.
 */
void gOrientEntityBasis(struct Self *arg)
{
 register struct Self *self __asm__("$17") = arg;
 struct Vec tmp;
 register s32 a __asm__("$4");
 register s32 b __asm__("$5");
 struct Vec *basis;
 s32 c, d;
 register s32 neg __asm__("$3");
 register s32 index __asm__("$4");
 register u8 *data __asm__("$2");
 if (self->flags & 0x200) {
  if (self->flags & 0x100) {
   self->pos.y = gGetGroundHeight(&self->pos, 0, 0, 0);
   a = self->unk140.z; b = self->unk140.x;
   goto fixed;
  }
  gGetGroundHeight(&self->pos, &tmp, 0, 0);
  tmp.x = (tmp.x + self->basis[1].x * 3) >> 2;
  tmp.y = (tmp.y + self->basis[1].y * 3) >> 2;
  tmp.z = (tmp.z + self->basis[1].z * 3) >> 2;
  func_800192AC(&tmp, &self->unk140, self->basis);
 } else if (self->flags & 0x100) {
  a = self->unk140.z; b = self->unk140.x;
 fixed:
  ((volatile struct Self *)self)->basis[2].y = 0;
  ((volatile struct Self *)self)->basis[1].z = 0;
  ((volatile struct Self *)self)->basis[1].x = 0;
  ((volatile struct Self *)self)->basis[0].y = 0;
  ((volatile struct Self *)self)->basis[1].y = 0x10000;
  __asm__("" : : : "memory");
  neg = b;
  __asm__("" : "+r"(neg));
  neg = -neg;
  self->basis[2].z = a;
  self->basis[0].x = a;
  self->basis[2].x = b;
  self->basis[0].z = neg;
 } else {
  c = self->unk130; d = self->unk136;
  basis = self->basis;
  if (self->flags >> 30) { c = self->unkB0; d = self->unkB4; }
  gBuildRotationMatrix(self->unk12a, c, d, basis);
  gGteLoadMatrix(basis);
  index = self->unk70 << 4;
  data = self->model->data;
  __asm__("" : "+r"(index));
  index += 8;
  gGteTransformVerticesWide(data + index, self->unk5c, 1);
 }
}
