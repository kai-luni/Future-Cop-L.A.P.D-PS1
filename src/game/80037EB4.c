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
extern s32 func_80066F78(struct Vec *, struct Vec *, s32, s32);
extern void func_800192AC(struct Vec *, struct Vec *, struct Vec *);
extern void func_800188D0(s32,s32,s32,struct Vec *);
extern void func_80010750(struct Vec *);
extern void func_80010234(u8 *, void *, s32);
void func_80037EB4(struct Self *arg)
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
   self->pos.y = func_80066F78(&self->pos, 0, 0, 0);
   a = self->unk140.z; b = self->unk140.x;
   goto fixed;
  }
  func_80066F78(&self->pos, &tmp, 0, 0);
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
  func_800188D0(self->unk12a, c, d, basis);
  func_80010750(basis);
  index = self->unk70 << 4;
  data = self->model->data;
  __asm__("" : "+r"(index));
  index += 8;
  func_80010234(data + index, self->unk5c, 1);
 }
}
