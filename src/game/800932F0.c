// Categories: Graphics > GPU > Command Queue | Platform > GPU
#include "game/types.h"

struct Context {
    u8 pad[8];
    void (*send)(void *, void *, int, int);
    u8 pad2[12];
    void *arg;
};
extern u8 gGpuDebugLevel[] __asm__("D_8009DDF2");
extern char gPutDrawEnvTraceFormat[] __asm__("D_80012AFC");
extern  void (*gLogPrintf)(char *, ...) __asm__("D_8009DDEC");
extern struct Context *gVramWriteCallback __asm__("D_8009DDE8");
extern void gBuildRectDrawList(u8 *, u8 *) __asm__("func_80093CE8");
extern void func_8008CD10(void *, void *, int);

/**
 * @brief Queues a command block: copies the object, page-aligns it and links it.
 *
 * @param arg Command object; the queue word is at +0x1C.
 * @return The command object (the pointer itself, as the retail code returns it).
 *
 * @par Confirmed
 * Debug hook at level 2, func_80093CE8(p, arg) prepares the 0x1C-offset block,
 * `*(u32 *)p |= 0xFFFFFF` (the low 24 bits become the list terminator),
 * func_8008CD10 copies into the library buffer and the library callback at
 * D_8009DDE8 runs with the assembled length.
 */
u8 *func_800932F0(u8 *arg)
{
    register u8 *base __asm__("$18") = gGpuDebugLevel;
    register u8 *saved __asm__("$17");
    u32 cond;
    register u32 mask __asm__("$4");
    register struct Context *ctx __asm__("$3");
    register u8 *p __asm__("$16");
    __asm__ volatile ("" : "=r"(base) : "0"(base));
    cond = base[0];
    __asm__ volatile ("" : "=r"(cond) : "0"(cond));
    saved = arg;
    if (cond >= 2)
        gLogPrintf(gPutDrawEnvTraceFormat, saved);
    p = saved + 0x1c;
    gBuildRectDrawList(p, saved);
    mask = 0xffffff;
    ctx = gVramWriteCallback;
    *(u32 *)p |= mask;
    ctx->send(ctx->arg, p, 0x40, 0);
    func_8008CD10(base + 0xe, saved, 0x5c);
    return saved;
}
