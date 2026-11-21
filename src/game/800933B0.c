// Categories: Platform > GPU | Graphics > 2D Drawing
#include "game/types.h"

struct Context {
    u8 pad[8];
    void (*send)(void *, void *, int, int);
    u8 pad2[12];
    void *arg;
};
extern u8 gGpuDebugLevel[] __asm__("D_8009DDF2");
extern char gDrawOTagEnvTraceFormat[] __asm__("D_80012B14");
extern  void (*gLogPrintf)(char *, ...) __asm__("D_8009DDEC");
extern struct Context *gVramWriteCallback __asm__("D_8009DDE8");
extern void gBuildRectDrawList(u8 *, u8 *) __asm__("func_80093CE8");
extern void func_8008CD10(void *, void *, int);
void gDrawOTagEnv(u32 arg0, u8 *arg1) __asm__("func_800933B0");


/**
 * @brief Builds a DRAWENV GPU packet, links it to an ordering table, and submits it (PsyQ DrawOTagEnv).
 *
 * Optionally logs "DrawOTagEnv(%08x,&%08x).." at debug level >= 2, packs GPU commands at
 * arg1+0x1C via func_80093CE8, patches the low 24-bit OT link, submits via D_8009DDE8->send, and caches the 0x5C-byte DRAWENV.
 *
 * @param arg0 Ordering table address linked into the low 24 bits of packet word *(arg1 + 0x1C).
 * @param arg1 Pointer to a 0x5C-byte DRAWENV structure containing the embedded packet buffer at +0x1C.
 *
 * @par Globals
 * D_8009DDF2 (debug level at [0], cached 0x5C-byte DRAWENV at +0xE), D_80012B14 (format string), D_8009DDEC (log callback), D_8009DDE8 (GPU driver context).
 * @par Control flow
 * Optional debug log if D_8009DDF2[0] >= 2, followed by straight-line packet build, OT pointer link, GPU submission, and DRAWENV copy.
 * @par Calls
 * func_80093CE8 (builds DRAWENV GPU command list at arg1+0x1C), func_8008CD10 (copies 0x5C bytes), indirect calls via D_8009DDEC and D_8009DDE8->send.
 * @par Side effects
 * Mutates packet buffer at arg1+0x1C, submits DMA/GPU command list, and updates cached DRAWENV at D_8009DDF2+0xE.
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s. Unknown: whether invoked indirectly or unreferenced PsyQ library unit.
 *
 * @par Environment path
 * PsyQ DrawOTagEnv implementation: copies a DRAWENV block via func_8008CD10,
 * builds its GPU primitive packet via func_80093CE8, logs when debug level is
 * set via gLogPrintf, and links/submits it to the ordering table.
 */
void gDrawOTagEnv(u32 arg0, u8 *arg1)
{
    register u8 *base __asm__("$19") = gGpuDebugLevel;
    register u8 *saved __asm__("$17");
    u32 cond;
    register u32 mask __asm__("$4");
    register struct Context *ctx __asm__("$3");
    register u8 *p __asm__("$16");
    __asm__ volatile ("" : "=r"(base) : "0"(base));
    cond = base[0];
    __asm__ volatile ("" : "=r"(cond) : "0"(cond));
    saved = arg1;
    if (cond >= 2)
        gLogPrintf(gDrawOTagEnvTraceFormat, arg0, saved);
    p = saved + 0x1c;
    gBuildRectDrawList(p, saved);
    mask = 0xffffff;
    ctx = gVramWriteCallback;
    *(u32 *)p = (*(u32 *)p & 0xff000000) | (arg0 & mask);
    ctx->send(ctx->arg, p, 0x40, 0);
    func_8008CD10(base + 0xe, saved, 0x5c);
}
