// Categories: Platform > Initialization
#include "game/dma_globals.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef struct Other {
    int pad;
    void *field4;
    char pad2[12];
    void *field14;
} Other;
extern char D_8009CC88[];
extern char D_8009CCC4[];
extern volatile u16 *D_8009DD14;
extern volatile u16 *D_8009DD18;
extern volatile u32 *gDmaControlPtr __asm__("D_8009DD1C");
extern Other *D_8009DD10;
extern void func_80091B1C(void *, unsigned int);
extern int func_80091B90(void *);
extern void func_800916EC(void);
extern void func_80091B80(void *);
extern void *func_80091BA0(void);
extern void *func_80091CC0(void);
extern void func_80091B48(void *);
extern void func_8008C9B0(void);

/**
 *  @brief Initializes a global subsystem and returns its state address.
 *
 *  The routine initializes register-backed state and a global structure,
 *  installs returned pointers, and invokes the subsystem setup helpers.
 *  @return The address D_8009CC88 on initialization, or null if already set.
 *  @par Globals
 *  D_8009CC88 contains the initialization flag; D_8009CCC4 anchors a
 *  pointer setup; D_8009DD14, D_8009DD18 and D_8009DD1C address registers.
 *  D_8009DD10 points to the structure receiving two helper results.
 *  @par Control flow
 *  A set flag returns immediately; a helper result conditionally triggers setup.
 *  @par Calls
 *  func_80091B1C initializes state; func_80091B90 tests it and
 *  func_800916EC handles a positive result. func_80091B80, func_80091BA0,
 *  func_80091CC0, func_80091B48 and func_8008C9B0 complete setup.
 *  @par Side effects
 *  Writes registers and global state and invokes initialization helpers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void *func_80091614(void)
{
    register char *p __asm__("$16") = D_8009CC88;
    volatile u16 *a;
    volatile u16 *b;
    __asm__("" : "+r"(p));
    if (*(u16 *)p != 0) return 0;
    a = D_8009DD14;
    b = D_8009DD18;
    *b = 0;
    *a = *b;
    *gDmaControlPtr = 0x33333333;
    func_80091B1C(p, 0x41a);
    if (func_80091B90(p + 0x38)) func_800916EC();
    p = D_8009CCC4;
    __asm__("" : "+r"(p));
    *(void **)p = p + 0xfdc;
    func_80091B80(p - 4);
    *(u16 *)(p - 0x3c) = 1;
    D_8009DD10->field14 = func_80091BA0();
    D_8009DD10->field4 = func_80091CC0();
    func_80091B48(D_8009DD10);
    p -= 0x3c;
    func_8008C9B0();
    return p;
}
