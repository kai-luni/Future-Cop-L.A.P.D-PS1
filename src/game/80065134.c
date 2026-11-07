typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct { int x, y, z; } Vec;
extern void func_800648E8(void *, u32, Vec *);
void func_80065134(void *obj, void *list, int a, int b, unsigned int index)
{
    Vec pos;
    u32 *p;
    int count = 0;
    int d;
    int delta = a - b;
    register unsigned int old __asm__("$7");
    u32 word;
    unsigned int type;
    register unsigned int t __asm__("$2");
    register unsigned int u __asm__("$3");
    register unsigned int arg __asm__("$4");
    p = (u32 *)((u8 *)obj + 0x5d0 + (((u16 *)((u8 *)obj + 0x3cc))[index] >> 4));
    pos = *(Vec *)obj;
    obj = &pos;
    do {
        word = *p++;
        type = ((u8 *)0x1f800000)[(word >> 15) & 0x7f] & 7;
        old = index;
        if (word & 1) {
            word &= ~1;
            count++;
            index = old + 1;
            if ((count & 3) == 0) index = old + 13;
        }
        if (type == 0) {
            word |= (word >> 11) & 1;
            goto call;
        }
        switch (type) {
        case 1:
            d = (old & 0xf0) - b;
            break;
        case 2:
            d = ((old << 4) & 0xf0) - a;
            break;
        case 3:
            u = (old << 4) & 0xf0;
            __asm__("" : "+r"(u));
            t = (old & 0xf0) + 16;
            __asm__("" : "+r"(t));
            u += t;
            __asm__("" : "+r"(u));
            u -= a;
            __asm__("" : "+r"(u));
            d = u - b;
            break;
        case 4:
            d = delta - ((old << 4) & 0xf0) + (old & 0xf0);
            break;
        }
        if (d >= 0) {
            if (!(word & 0x800)) continue;
            if (d > 0) goto call;
        }
        if (!(word & 0x1000)) continue;
call:
        arg = old >> 4;
        __asm__("" : "+r"(arg));
        arg = old + arg;
        __asm__("" : "+r"(arg));
        t = arg << 1;
        __asm__("" : "+r"(t));
        arg = arg + t;
        __asm__("" : "+r"(arg));
        func_800648E8((u8 *)list + arg * 8, word, obj);
    } while (count < 16);
}
