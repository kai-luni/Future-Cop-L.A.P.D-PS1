extern volatile unsigned char *D_8009CB6C;
extern volatile unsigned char *D_8009CB70;
extern volatile unsigned char *D_8009CB74;
extern volatile unsigned char *D_8009CB78;
extern unsigned char D_8009C8C5;
extern int D_8009CA6C[];
extern int D_8009C96C[];
extern int D_8009C8CC[];
extern int D_8009C8B0, D_8009C8B4, D_8009C8B8, D_8009C8BC;
extern unsigned char D_8009CB84[];
extern unsigned char D_800A3E50[], D_800A3E58[], D_800A3E60[];
extern char D_800124B4[], D_800124C0[], D_800124DC[], D_800124F0[];
extern void func_8008E000();
extern void func_8008FF08();

#define COPY(dst) \
    { register unsigned char *d __asm__("$3") = dst; unsigned char *s = buf; int n; \
      if (d) { for (n = 7; n != -1; n--) { *d++ = *s++; } } }
#define COPY4(dst) \
    { register unsigned char *d __asm__("$4") = dst; unsigned char *s = buf; int n; \
      if (d) { for (n = 7; n != -1; n--) { *d++ = *s++; } } }

int func_8008E7A0(void)
{
    volatile unsigned char mode;
    unsigned char buf[8];
    int s0;
    int s1;
    volatile unsigned char *p;

    *D_8009CB6C = 1;
    p = D_8009CB78;
    mode = *p & 7;
    if (mode == 0) {
        return 0;
    }
    s1 = 0;
    while (mode != (*p & 7)) {
        mode = *p & 7;
    }
    for (s0 = 0; s0 < 8; s0++) {
        if (!(*D_8009CB6C & 0x20)) break;
        buf[s0] = *D_8009CB70;
    }
    {
        int i = s0;
        while (i < 8) { buf[i] = 0; i++; }
    }
    *D_8009CB6C = 1;
    *D_8009CB78 = 7;
    *D_8009CB74 = 7;
    if (mode == 3) {
        register int index __asm__("$2");
        register int value __asm__("$2");
        index = D_8009C8C5 << 2;
        __asm__("lui $1, %%hi(D_8009CA6C)\n\taddu $1, $1, %1\n\tlw %0, %%lo(D_8009CA6C)($1)" : "=r"(value) : "r"(index) : "$1");
        if (!value) goto skip_input;
    }
    {
        if (!(D_8009C8B4 & 0x10) && (buf[0] & 0x10)) {
            D_8009C8BC = D_8009C8BC + 1;
        }
        {
            int a = buf[0];
            int b = buf[1];
            __asm__("" : "+r"(a), "+r"(b));
            a &= 0xff;
            s1 = a & 0x1D;
            D_8009C8B4 = a;
            D_8009C8B8 = b;
        }
    }
skip_input:
    if (mode == 5 && D_8009C8B0 > 0) {
        func_8008E000(D_800124B4);
        if (D_8009C8B0 > 0) {
            register int index __asm__("$2");
            register int val __asm__("$5");
            register int a __asm__("$6");
            register int b __asm__("$7");
            index = D_8009C8C5;
            a = D_8009C8B4;
            b = D_8009C8B8;
            __asm__("" : "+r"(index), "+r"(a), "+r"(b));
            index <<= 2;
            __asm__("lui %0, %%hi(D_8009C8CC)\n\taddu %0, %0, %1\n\tlw %0, %%lo(D_8009C8CC)(%0)" : "=&r"(val) : "r"(index));
            func_8008E000(D_800124C0, val, a, b);
        }
    }
    {
        static void *const table[] = { &&case1, &&case2, &&case3, &&case4, &&case5 };
        register unsigned int value __asm__("$2") = mode;
        register unsigned int index __asm__("$3");
        register unsigned int target __asm__("$2");
        __asm__("" : "+r"(value));
        index = value - 1;
        if (index >= 5) goto other;
        target = index << 2;
        __asm__("lui $1, %%hi(%1)\n\taddu $1, $1, %0\n\tlw %0, %%lo(%1)($1)" : "+r"(target) : "i"(table) : "$1");
        goto *(void *)target;
    case3:
        if (s1) {
            register int flag __asm__("$2") = 5;
            register unsigned char *status __asm__("$3");
            __asm__("" : "+r"(flag));
            status = D_8009CB84;
            __asm__("" : "+r"(status));
            *status = flag;
            COPY(D_800A3E50);
            return 2;
        }
        {
            register int index __asm__("$2");
            register int value __asm__("$2");
            index = D_8009C8C5 << 2;
            __asm__("lui $1, %%hi(D_8009C96C)\n\taddu $1, $1, %1\n\tlw %0, %%lo(D_8009C96C)($1)" : "=r"(value) : "r"(index) : "$1");
            if (value != 0) {
                register int flag __asm__("$2") = 3;
                register unsigned char *status __asm__("$3");
                __asm__("" : "+r"(flag));
                status = D_8009CB84;
                __asm__("" : "+r"(status));
                *status = flag;
                COPY(D_800A3E50);
                return 1;
            }
        }
        {
            register unsigned char *status __asm__("$3") = D_8009CB84;
            __asm__("" : "+r"(status));
            *status = 2;
        }
        COPY(D_800A3E50);
        return 2;
    case2:
        {
            register int state __asm__("$2") = 2;
            if (s1) state = 5;
            __asm__("" : "+r"(state));
            __asm__("lui $1, %%hi(D_8009CB84)\n\tsb %0, %%lo(D_8009CB84)($1)" : : "r"(state) : "$1", "memory");
        }
        COPY(D_800A3E50);
        return 2;
    case1:
        if (s1 && s0 == 1) s1 = 0;
        {
            register int state __asm__("$3") = 1;
            register unsigned char *status __asm__("$2");
            if (s1) state = 5;
            __asm__("" : "+r"(state));
            status = D_8009CB84;
            __asm__("" : "+r"(status));
            status[1] = state;
        }
        COPY(D_800A3E58);
        *D_8009CB6C = 0;
        *(unsigned char *)D_8009CB78 = 0;
        return 4;
    case4:
        {
        register unsigned char *dest __asm__("$4") = D_800A3E60;
        register unsigned char *status __asm__("$2") = D_8009CB84;
        __asm__("" : "+r"(dest), "+r"(status));
        {
            register unsigned char *source __asm__("$5");
            register int value __asm__("$3");
            int n;
            status[2] = 4;
            __asm__ volatile("" : : : "memory");
            value = status[2];
            __asm__("" : "+r"(value));
            source = buf;
            __asm__("" : "+r"(value), "+r"(source));
            status[1] = value;
            __asm__ volatile("" : : : "memory");
            if (dest) { for (n = 7; n != -1; n--) { *dest++ = *source++; } }
        }
        }
        COPY(D_800A3E58);
        return 4;
    case5:
        {
        register unsigned char *dest __asm__("$4") = D_800A3E50;
        register unsigned char *status __asm__("$2") = D_8009CB84;
        __asm__("" : "+r"(dest), "+r"(status));
        {
            register unsigned char *source __asm__("$5");
            register int value __asm__("$3");
            int n;
            status[1] = 5;
            __asm__ volatile("" : : : "memory");
            value = status[1];
            __asm__("" : "+r"(value));
            source = buf;
            __asm__("" : "+r"(value), "+r"(source));
            status[0] = value;
            __asm__ volatile("" : : : "memory");
            if (dest) { for (n = 7; n != -1; n--) { *dest++ = *source++; } }
        }
        }
        COPY(D_800A3E58);
        return 6;
    other:
        func_8008FF08(D_800124DC);
        {
            register int m __asm__("$5");
            register char *p __asm__("$4");
            __asm__("lbu %0, %2\n\tlui %1, %%hi(D_800124F0)" : "=r"(m), "=r"(p) : "m"(mode));
            func_8008E000(p + 0x24f0, m);
        }
        return 0;
    }
}
