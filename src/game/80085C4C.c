// Categories: Graphics > Video Layer > Playback Context | Platform > MDEC | Graphics > GPU > Draw Environments | Resources > Movie Playback
#include "game/types.h"
typedef struct {
 u8 pad00[0x24]; void *a24; void *a28; void *a2c;
 u8 pad30[0xcc]; u8 fc, fd; u8 padfe[8]; s16 f106; u8 pad108[2]; s16 f10a; u8 pad10c[4]; u8 f110, f111; u8 pad112[2]; s32 f114; u8 pad118[8]; s32 f120, f124, f128, f12c;
} Ctx;
extern Ctx *gDecodeJob __asm__("D_8009F4BC");
extern int D_8009F4C4, D_8009F508;
extern int gStripScaleEnable __asm__("D_8009F4C0");
extern char D_80085074[];
extern void *func_8001E4C0(void *, int, unsigned int);
extern void func_80085AC4(int,int);
extern void func_80092140(void *,int,int,int,int);
extern void func_800921F4(void *,int,int,int,int);
extern void func_80091270(int), func_800932F0(void *), func_800934BC(void *), func_80092BF4(int), func_80095E90(int);
extern void func_8008C9C0(void), func_8008C9E0(void), func_80085F1C(void);
extern int func_8009612C(void *), func_80096108(int), func_80092B94(int);
/**
 * @brief Initialises the video decoder context: clears the 0x130-byte block,
 *        registers four draw/display environments for a 320x240 picture, stores
 *        the three ring pointers and optionally hooks the decode callbacks.
 *
 * @param ctx Decoder context, 0x130 bytes. Returns 0 when it is null.
 * @param a1  First ring pointer, stored at ctx+0x28.
 * @param a2  Second ring pointer, stored at ctx+0x2C.
 * @param a3  Third ring pointer, stored at ctx+0x24.
 * @param a4  Callback mode, stored at ctx+0x12C; non-zero registers the three
 *            decode callbacks and sets D_8009F508.
 * @param a5  Byte forwarded to func_80085AC4 (only the low 8 bits are used).
 * @param a6  Non-zero applies the current draw/display environment and flushes
 *            the GPU command queue before anything else is stored.
 *
 * @return 1 when the context was initialised, 0 when @c ctx or any of the three
 *         pointers was null (0x80085C80..0x80085C98).
 *
 * @par Globals
 * - D_8009F4BC (0x8009F4BC, $gp 0x2A8): the global playback context; overwritten
 *   with @c ctx and then used for every field access.
 * - D_8009F4C0 (0x8009F4C0, $gp 0x2AC): compared with 1 to select the
 *   interlaced flag pair and to set D_8009F508.
 * - D_8009F4C4 (0x8009F4C4, $gp 0x2B0): must be 0 together with the above for
 *   the flag pair to be set.
 * - D_8009F508 (0x8009F508): set to 1 or 0 depending on a4 and D_8009F4C0.
 * - D_80085074 (0x80085074): the address of func_80085074, passed as a function
 *   pointer to func_8009612C, the channel 1 registration.
 *
 * @par Control flow
 * Confirmed:
 * 1. Null checks for ctx, a1, a2 and a3, then func_8001E4C0(ctx, 0, 0x130)
 *    (memset) at 0x80085CA0.
 * 2. D_8009F4BC = ctx; func_80085AC4((unsigned char)a5, 0).
 * 3. Four environment blocks, two of 92 bytes and two of 20 bytes (the offsets
 *    differ by exactly 0x5C and 0x14):
 *    func_80092140(ctx+0x34, 0, 0, 0x140, 0xF0),
 *    func_800921F4(ctx+0xEC, 0, 0, 0x140, 0xF0),
 *    func_80092140(ctx+0x90, 0, 0x100, 0x140, 0xF0),
 *    func_800921F4(ctx+0x100, 0, 0x100, 0x140, 0xF0). The height is passed on
 *    the stack as 0xF0 in all four calls.
 * 4. If D_8009F4C0 == 1 and D_8009F4C4 == 0 then ctx+0xFD = 1 and ctx+0x111 = 1,
 *    otherwise both are cleared.
 * 5. ctx+0xFC, ctx+0x110, ctx+0x114, ctx+0xF6 (s16) and ctx+0x10A (s16) are
 *    cleared.
 * 6. When a6 is non-zero: func_80091270(0), then func_800932F0 with
 *    ctx + 0x34 + (1 - ctx+0x114) * 92 and func_800934BC with
 *    ctx + 0xEC + (1 - ctx+0x114) * 20 - the two 92-byte and two 20-byte blocks
 *    are double buffered and the flag selects the pair - and finally
 *    func_80092BF4(1).
 * 7. ctx+0x28 = a1, ctx+0x2C = a2, ctx+0x24 = a3.
 * 8. func_80095E90(0); func_8008C9C0(); ctx+0x12C = a4.
 * 9. When a4 != 0: ctx+0x120 = func_8009612C(&D_80085074),
 *    ctx+0x124 = func_80096108(0), ctx+0x128 = func_80092B94(0) and, if
 *    D_8009F4C0 == 1, D_8009F508 = 1. Otherwise the same three calls with a null
 *    first argument and D_8009F508 = 0.
 * 10. func_8008C9E0(); func_80085F1C(); return 1.
 *
 * @par Calls
 * func_8001E4C0, func_80085AC4, func_80092140 (twice), func_800921F4 (twice),
 * func_80091270, func_800932F0, func_800934BC, func_80092BF4, func_80095E90,
 * func_8008C9C0, func_8008C9E0, func_8009612C, func_80096108, func_80092B94,
 * func_80085F1C.
 *
 * @par Side effects
 * Clears and fills the caller's 0x130-byte context, replaces the global playback
 * context pointer, registers or clears the two MDEC decode callbacks and the
 * completion callback, sets D_8009F508, and (when a6 is set) applies a draw and
 * display environment plus one queue flush. Interrupts stay enabled only between
 * func_8008C9C0 and func_8008C9E0.
 *
 * @par Likely role
 * The "open playback context" half of func_80087EA0: that caller allocates one
 * ring buffer and a3 = 0x10000 (0x6DE0 in its narrow variant), then passes the
 * ring, ring + a3 and ring + 2 * a3 as a1, a2 and a3 with a4 = 1, a5 = narrow and
 * a6 = 0, which fits a triple-buffered 320x240 decoder. The 92-byte and 20-byte
 * records match the sizes of the libgs GsDRAWENV (0x5C) and GsDISPENV (0x14)
 * structures, and func_80092140 / func_800921F4 fill a rectangle in each.
 * Interpretation marked Likely: the identification of the records with those
 * two libgs structures.
 *
 * @par Unknown
 * - The meaning of the ctx+0xFD / ctx+0x111 pair (both set together only when
 *   D_8009F4C0 == 1 and D_8009F4C4 == 0; interlacing is the obvious guess).
 * - What func_80085AC4 does with the byte from a5.
 * - Why the y origin is 0x100 for the second pair of environments; nothing in
 *   the code reads it back.
 *
 * @par Callers
 * func_80087EA0, recorded at 0x80087EA4 (the second instruction of its
 * prologue; the direct call is at 0x8008803C). func_80087EA0 itself is called
 * only by func_80089D44.
 */
int func_80085C4C(Ctx *ctx, void *a1, void *a2, void *a3, int a4, int a5, int a6)
{
 if (ctx) {
 if (a1 && a2 && a3) {
 func_8001E4C0(ctx,0,0x130);
 gDecodeJob=ctx;
 func_80085AC4((u8)a5,0);
 func_80092140((u8 *)gDecodeJob+0x34,0,0,0x140,0xf0);
 func_800921F4((u8 *)gDecodeJob+0xec,0,0,0x140,0xf0);
 func_80092140((u8 *)gDecodeJob+0x90,0,0x100,0x140,0xf0);
 func_800921F4((u8 *)gDecodeJob+0x100,0,0x100,0x140,0xf0);
 if (gStripScaleEnable == 1 && D_8009F4C4 == 0) {
  gDecodeJob->fd=1; gDecodeJob->f111=1;
 } else { gDecodeJob->fd=0; gDecodeJob->f111=0; }
 gDecodeJob->fc=0; gDecodeJob->f110=0;
 gDecodeJob->f114=0; *(s16 *)((u8 *)gDecodeJob+0xf6)=0; gDecodeJob->f10a=0;
 if (a6) {
  func_80091270(0);
  func_800932F0((u8 *)gDecodeJob+0x34+(1-gDecodeJob->f114)*92);
  func_800934BC((u8 *)gDecodeJob+0xec+(1-gDecodeJob->f114)*20);
  func_80092BF4(1);
 }
 gDecodeJob->a28=a1; gDecodeJob->a2c=a2; gDecodeJob->a24=a3;
 func_80095E90(0); func_8008C9C0(); gDecodeJob->f12c=a4;
 if (a4) {
  gDecodeJob->f120=func_8009612C(D_80085074);
  gDecodeJob->f124=func_80096108(0);
  gDecodeJob->f128=func_80092B94(0);
  if (gStripScaleEnable==1) D_8009F508=1;
 } else {
  gDecodeJob->f120=func_8009612C(0);
  gDecodeJob->f124=func_80096108(0);
  gDecodeJob->f128=func_80092B94(0);
  D_8009F508=0;
 }
 func_8008C9E0(); func_80085F1C(); return 1;
 }
 }
 return 0;
}
