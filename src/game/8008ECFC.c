typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern int D_800A3E68, D_800A3E6C;
extern char *D_800A3E70;
extern char D_80012510[], D_80012488[], D_80012498[];
extern int D_8009C8CC[], D_8009C94C[];
extern u8 D_8009CB84[], D_8009C8C5;
extern void (*D_8009C8AC)(int, void *);
extern void (*D_8009C8A8)(int, void *);
extern volatile u8 *D_8009CB6C;
extern u8 D_800A3E58[];
extern u8 D_800A3E50[];

extern int func_80091270(int);
extern void func_8008FF08(char *);
extern void func_8008E000(char *, char *, int, int);
extern void func_8008F6D8(void);
extern int func_800915D4(void);
extern int func_8008E7A0(void);
extern void dummy_5args(int, int, int, int, int);

s32 func_8008ECFC(s32 mode, u8 *result)
{
    register int s0 __asm__("$16");
    register int s1 __asm__("$17");
    register u8 *state __asm__("$18");
    register s32 two __asm__("$19");
    register int *table __asm__("$20");
    register u8 *state1 __asm__("$21");
    register s32 saved_mode __asm__("$22") = mode;
    register u8 *saved_result __asm__("$23") = result;
    int *other;
    char *msg;
    int ret;
    int now;
    int status;

    __asm__("" : "+r"(saved_mode));
    __asm__("" : "+r"(saved_result));

    now = func_80091270(-1);
    __asm__("lui $30, %%hi(D_8009C8CC)\n\taddiu $30, $30, %%lo(D_8009C8CC)" : "=r"(other));
    table = D_8009C94C;
    state = D_8009CB84;
    state1 = state + 1;
    two = 2;
    D_800A3E68 = now + 0x3c0;
    msg = D_80012510;
    D_800A3E6C = 0;
    D_800A3E70 = msg;

loop:
    if (D_800A3E68 < func_80091270(-1)) goto timeout;
    {
        register int cur __asm__("$2");
        register int next __asm__("$2");
        register int old __asm__("$3");
        cur = D_800A3E6C;
        __asm__("" : "+r"(cur));
        old = cur;
        next = cur + 1;
        __asm__("" : "+r"(old), "+r"(next));
        D_800A3E6C = next;
        if (0x3c0000 < old) {
        timeout:
            func_8008FF08(D_80012488);
            {
                register int first __asm__("$4") = state[0];
                register int second __asm__("$2") = state[1];
                register char *m __asm__("$5") = D_800A3E70;
                register int fifth __asm__("$3");
                register int index __asm__("$2");
                register int fourth __asm__("$7");
                register int third __asm__("$6");

                __asm__("" : "+r"(first), "+r"(second), "+r"(m));
                second = (second << 2) + (int)table;
                __asm__("" : "+r"(second));
                first <<= 2;
                __asm__("" : "+r"(first));
                fifth = *(int *)second;
                index = D_8009C8C5;
                __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                first = first + (int)table;
                __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));
                index = (index << 2) + (int)other;
                __asm__("" : "+r"(first), "+r"(fifth), "+r"(index));

                __asm__("sw $3, 16($sp)");
                third = *(int *)index;
                fourth = *(int *)first;
                __asm__("" : "+r"(third), "+r"(fourth));
                func_8008E000(D_80012498, m, third, fourth);
            }
            func_8008F6D8();
            ret = -1;
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        return -1;
    }

    if (func_800915D4() != 0) {
        {
            register int v __asm__("$2") = *D_8009CB6C;
            __asm__("" : "+r"(v));
            s1 = v & 3;
        }
inner_loop:
        s0 = func_8008E7A0();
        if (s0 != 0) {
            if (s0 & 4) {
                if (D_8009C8AC != 0) {
                    D_8009C8AC(*state1, D_800A3E58);
                }
            }
            {
                register int m2 __asm__("$2");
                m2 = s0 & 2;
                if (m2) {
                    if (D_8009C8A8 != 0) {
                        register int a0 __asm__("$4");
                        a0 = *state;
                        __asm__("" : "+r"(a0));
                        D_8009C8A8(a0, D_800A3E50);
                    }
                }
            }
            goto inner_loop;
        }
        *D_8009CB6C = s1;
    }

    {
        register int sv __asm__("$2");
        sv = state[0];
        __asm__("" : "+r"(sv));
        status = sv & 0xFF;
    }
    if (status == two || status == 5) {
        register u8 *src __asm__("$4");
        register u8 *dst __asm__("$5");
        state[0] = two;
        dst = saved_result;
        src = D_800A3E50;
        __asm__("" : "+r"(dst), "+r"(src));
        if (dst != 0) {
            int i = 7;
            do {
                *dst++ = *src++;
            } while (--i != -1);
        }
        return status;
    }

    if (saved_mode == 0) goto loop;

    if (0) dummy_5args(0, 0, 0, 0, 0);

    return 0;
}
