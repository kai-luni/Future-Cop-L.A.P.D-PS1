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
