#include "game/types.h"
typedef struct {
 u8 pad00[0x24]; void *a24; void *a28; void *a2c;
 u8 pad30[0xcc]; u8 fc, fd; u8 padfe[8]; s16 f106; u8 pad108[2]; s16 f10a; u8 pad10c[4]; u8 f110, f111; u8 pad112[2]; s32 f114; u8 pad118[8]; s32 f120, f124, f128, f12c;
} Ctx;
extern Ctx *D_8009F4BC;
extern int D_8009F4C0, D_8009F4C4, D_8009F508;
extern char D_80085074[];
extern void *func_8001E4C0(void *, int, unsigned int);
extern void func_80085AC4(int,int);
extern void func_80092140(void *,int,int,int,int);
extern void func_800921F4(void *,int,int,int,int);
extern void func_80091270(int), func_800932F0(void *), func_800934BC(void *), func_80092BF4(int), func_80095E90(int);
extern void func_8008C9C0(void), func_8008C9E0(void), func_80085F1C(void);
extern int func_8009612C(void *), func_80096108(int), func_80092B94(int);
int func_80085C4C(Ctx *ctx, void *a1, void *a2, void *a3, int a4, int a5, int a6)
{
 if (ctx) {
 if (a1 && a2 && a3) {
 func_8001E4C0(ctx,0,0x130);
 D_8009F4BC=ctx;
 func_80085AC4((u8)a5,0);
 func_80092140((u8 *)D_8009F4BC+0x34,0,0,0x140,0xf0);
 func_800921F4((u8 *)D_8009F4BC+0xec,0,0,0x140,0xf0);
 func_80092140((u8 *)D_8009F4BC+0x90,0,0x100,0x140,0xf0);
 func_800921F4((u8 *)D_8009F4BC+0x100,0,0x100,0x140,0xf0);
 if (D_8009F4C0 == 1 && D_8009F4C4 == 0) {
  D_8009F4BC->fd=1; D_8009F4BC->f111=1;
 } else { D_8009F4BC->fd=0; D_8009F4BC->f111=0; }
 D_8009F4BC->fc=0; D_8009F4BC->f110=0;
 D_8009F4BC->f114=0; *(s16 *)((u8 *)D_8009F4BC+0xf6)=0; D_8009F4BC->f10a=0;
 if (a6) {
  func_80091270(0);
  func_800932F0((u8 *)D_8009F4BC+0x34+(1-D_8009F4BC->f114)*92);
  func_800934BC((u8 *)D_8009F4BC+0xec+(1-D_8009F4BC->f114)*20);
  func_80092BF4(1);
 }
 D_8009F4BC->a28=a1; D_8009F4BC->a2c=a2; D_8009F4BC->a24=a3;
 func_80095E90(0); func_8008C9C0(); D_8009F4BC->f12c=a4;
 if (a4) {
  D_8009F4BC->f120=func_8009612C(D_80085074);
  D_8009F4BC->f124=func_80096108(0);
  D_8009F4BC->f128=func_80092B94(0);
  if (D_8009F4C0==1) D_8009F508=1;
 } else {
  D_8009F4BC->f120=func_8009612C(0);
  D_8009F4BC->f124=func_80096108(0);
  D_8009F4BC->f128=func_80092B94(0);
  D_8009F508=0;
 }
 func_8008C9E0(); func_80085F1C(); return 1;
 }
 }
 return 0;
}
