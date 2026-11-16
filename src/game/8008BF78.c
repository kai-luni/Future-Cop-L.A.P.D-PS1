// Categories: Input > Controller Communication
typedef signed int s32;
typedef unsigned short u16;
struct E { char pad[0xF0]; };
struct H { char pad[10]; u16 v; };
extern s32 D_8009C768;
extern s32 D_8009C76C;
extern s32 D_8009C77C;
extern s32 D_8009C780[];
extern struct H *D_8009C7C0;
extern struct E D_800A3C20[];
extern void func_8008C77C(struct E *);
extern void func_8008C06C(struct E *);
extern s32 func_8008A6CC(struct E *);

/**
 *  @brief Process controller entries from the current index until traversal ends.
 *
 *  Uses the initial argument to skip processing, clear a table entry, or run
 *  two entry handlers; subsequent iterations use a nonzero processing value.
 *  @param arg Initial action selector: -9 skips, zero clears, others process.
 *  @par Globals
 *  D_800A3C20 holds entries; D_8009C768 is the current index and D_8009C77C
 *  bounds traversal. D_8009C780 holds per-entry values; D_8009C76C and
 *  D_8009C7C0->v are cleared on each iteration.
 *  @par Control flow
 *  Loops until the incremented index exceeds the bound or the probe succeeds.
 *  @par Calls
 *  func_8008C77C and func_8008C06C process an entry; func_8008A6CC probes
 *  the next entry when it is within the bound.
 *  @par Side effects
 *  Advances the global index, clears shared fields, and may process entries.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008BF78(s32 arg)
{
    register s32 a __asm__("$5") = arg;
    register s32 base __asm__("$17") = (s32)D_800A3C20;
    register s32 neg __asm__("$19") = -9;
    register s32 *tab __asm__("$18") = D_8009C780;
    s32 done;
    register s32 zero __asm__("$0");
    __asm__("" : "=r"(zero));
    __asm__("" : "+r"(a));
    __asm__("" : "+r"(base), "+r"(tab), "+r"(neg));
    do {
        struct E *p = (struct E *)(D_8009C768 * 240 + base);
        if (a != neg) {
            if (!a) {
                tab[D_8009C768] = zero;
            } else {
                func_8008C77C(p);
                func_8008C06C(p);
            }
        }
        D_8009C76C = 0;
        D_8009C7C0->v = 0;
        D_8009C768++;
        done = (D_8009C77C < D_8009C768) ? 1 : func_8008A6CC((struct E *)(D_8009C768 * 240 + base));
        a = 0xFFFF;
    } while (!done);
}
