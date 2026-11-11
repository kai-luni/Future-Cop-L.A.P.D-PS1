typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern int D_8009C8B0, D_8009CAEC[], D_8009C9EC[];
extern int D_8009C8CC[], D_8009C94C[];
extern u8 D_8009C8C0[], D_8009C8C4, D_8009C8C5, D_8009CB84[];
extern volatile u8 *D_8009CB6C, *D_8009CB70, *D_8009CB74;
extern int D_800A3E68, D_800A3E6C;
extern char *D_800A3E70;
extern char D_80012524[], D_8001252C[], D_8001253C[], D_80012488[], D_80012498[];
extern u8 D_800A3E50[], D_800A3E58[];
extern void (*D_8009C8AC)(int, void *), (*D_8009C8A8)(int, void *);
extern void func_8008E000(char *, ...), func_8008ECFC(int, int), func_8008FF08(char *), func_8008F6D8(void);
extern int func_80091270(int), func_800915D4(void), func_8008E7A0(void);
extern void dummy_5args(int, int, int, int, int);

int func_8008F244(int arg0, u8 *arg1, u8 *arg2, int arg3)
{
    register u8 *param __asm__("$16") = arg1;
    register u8 *saved_result __asm__("$22") = arg2;
    register u8 *state __asm__("$18") = (u8 *)arg3;
    register int s1 __asm__("$17") = arg0;
    register int *table __asm__("$19");
    register u8 *state1 __asm__("$20");
    register int *other __asm__("$21");
    int i;
    int ret;
    register int zero __asm__("$4");

    if (D_8009C8B0 >= 2) {
        register unsigned int index __asm__("$2") = (s1 & 255) << 2;
        register char *entry __asm__("$5");
        __asm__("" : "+r"(saved_result));
        __asm__("lui %0,%%hi(D_8009C8CC)\n\taddu %0,%0,%1\n\tlw %0,%%lo(D_8009C8CC)(%0)" : "=&r"(entry) : "r"(index));
        func_8008E000(D_80012524, entry);
    }
    {
        register int value __asm__("$2");
        register int offset __asm__("$3");
        __asm__("andi %0,%2,255\n\tsll %1,%0,2\n\tlui %0,%%hi(D_8009CAEC)\n\taddu %0,%0,%1\n\tlw %0,%%lo(D_8009CAEC)(%0)" : "=&r"(value), "=&r"(offset) : "r"(s1));
        zero = 0;
        if (value != 0) {
            __asm__("" : "+r"(zero));
            if (param == 0) {
                if (D_8009C8B0 > 0) {
                    register char *entry __asm__("$5");
                    __asm__("lui %0,%%hi(D_8009C8CC)\n\taddu %0,%0,%1\n\tlw %0,%%lo(D_8009C8CC)(%0)" : "=&r"(entry) : "r"(offset));
                    func_8008E000(D_8001252C, entry);
                }
                return -2;
            }
        }
    }
    func_8008ECFC(zero, 0);
    {
        register int cmd_lo __asm__("$3") = s1 & 255;
        register int two __asm__("$2") = 2;
        register int fourteen __asm__("$2");
        __asm__("" : "+r"(cmd_lo), "+r"(two));
        if (cmd_lo == two) {
            register int idx __asm__("$4") = 0;
            register u8 *p __asm__("$2");
            do {
                p = param + idx;
                __asm__ volatile(".set noat\n\tlbu %0,0(%0)\n\tlui $1,%%hi(D_8009C8C0)\n\taddu $1,$1,%1\n\tsb %0,%%lo(D_8009C8C0)($1)\n\t.set at" : "+r"(p) : "r"(idx) : "memory");
                idx++;
            } while (idx < 4);
            cmd_lo = s1 & 255;
        }
        fourteen = 14;
        __asm__("" : "+r"(cmd_lo), "+r"(fourteen));
        if (cmd_lo == fourteen) {
            D_8009C8C4 = *param;
        }
        {
            register u8 *st __asm__("$5");
            register int off __asm__("$4");
            register int *c9ec __asm__("$3");
            register int v __asm__("$2");
            __asm__ volatile("" : "+r"(cmd_lo));
            st = D_8009CB84;
            off = cmd_lo << 2;
            __asm__("" : "+r"(st), "+r"(off));
            st[0] = 0;
            __asm__("lui %0,%%hi(D_8009C9EC)\n\taddu %0,%0,%1\n\tlw %0,%%lo(D_8009C9EC)(%0)" : "=&r"(v) : "r"(off));
            c9ec = D_8009C9EC;
            if (v != 0) {
                __asm__("" : "+r"(c9ec));
                st[1] = 0;
            }
            {
                register volatile u8 *p6c __asm__("$2") = D_8009CB6C;
                __asm__("" : "+r"(p6c), "+r"(c9ec));
                *p6c = 0;
            }
            {
                register int *caec_ptr __asm__("$3");
                register int count __asm__("$2");
                v = (int)c9ec + 0x100;
                __asm__("" : "+r"(v), "+r"(off));
                caec_ptr = (int *)(off + v);
                count = *caec_ptr;
                {
                    register int idx __asm__("$4") = 0;
                    if (count > 0) {
                        register int *lim_ptr __asm__("$5") = caec_ptr;
                        register u8 *p __asm__("$2");
                        __asm__("" : "+r"(idx), "+r"(lim_ptr));
                        do {
                            p = param + idx;
                            *D_8009CB74 = *p;
                            idx++;
                        } while (idx < *lim_ptr);
                    }
                }
            }
        }
    }
    {
        register volatile u8 *p70 __asm__("$2") = D_8009CB70;
        __asm__("" : "+r"(p70));
        D_8009C8C5 = s1;
        *p70 = s1;
    }
    if (state != 0) return 0;
    {
        register int now __asm__("$2") = func_80091270(-1);
        register u8 *st __asm__("$4");
        register int st0 __asm__("$3");
        register char *msg __asm__("$2");
        now += 0x3c0;
        __asm__("" : "+r"(now));
        st = D_8009CB84;
        __asm__("" : "+r"(st), "+r"(now));
        D_800A3E68 = now;
        __asm__ volatile(".set noat\n\tlui $1,%%hi(D_800A3E6C)\n\tsw $0,%%lo(D_800A3E6C)($1)\n\t.set at" : : : "memory");
        st0 = st[0];
        __asm__("" : "+r"(st0));
        msg = D_8001253C;
        __asm__("" : "+r"(st0), "+r"(msg));
        D_800A3E70 = msg;
        if (st0 != 0) goto after_loop;
        other = D_8009C8CC;
        table = D_8009C94C;
        state = st;
        __asm__("" : "+r"(state));
        state1 = state + 1;
    }
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
        param = (u8 *)func_8008E7A0();
        if (param != 0) {
            if ((int)param & 4) {
                if (D_8009C8AC != 0) {
                    D_8009C8AC(*state1, D_800A3E58);
                }
            }
            {
                register int m2 __asm__("$2");
                m2 = (int)param & 2;
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
    if (state[0] == 0) goto loop;

after_loop:
    {
        register u8 *dst __asm__("$6") = saved_result;
        register u8 *src __asm__("$4") = D_800A3E50;
        __asm__("" : "+r"(dst), "+r"(src));
        if (dst != 0) {
            int j = 7;
            do {
                *dst++ = *src++;
            } while (--j != -1);
        }
    }
    {
        register int res __asm__("$4") = 0;
        register u8 *st __asm__("$2") = D_8009CB84;
        __asm__("" : "+r"(res), "+r"(st));
        if (st[0] == 5) {
            res = -1;
            __asm__("" : "+r"(res));
        }
        if (0) dummy_5args(0, 0, 0, 0, 0);
        return res;
    }
}
