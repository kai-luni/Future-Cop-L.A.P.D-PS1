typedef int s32;
typedef unsigned char u8;
typedef struct { volatile s32 state; volatile s32 pos; s32 value; s32 copy; s32 mode; s32 other; } Entry;
typedef struct { u8 loc[4]; s32 value; } File;
extern Entry D_800A6D78[];
extern char D_800A6DD8[];
extern File D_800A6E20[];
extern s32 func_8001CE10(void);
extern s32 func_8001DC48(s32, s32, void *, char *);
extern s32 func_8001C478(s32);
extern s32 func_8008FF20(File *, char *);
extern s32 func_8001C588(File *);
s32 func_8001D424(s32 arg0, s32 arg1, void *arg2)
{
    s32 i, j, ret;
    register s32 mode __asm__("$4");
    register s32 argument __asm__("$5");
    register s32 three __asm__("$2");
    Entry *entry;
    register char *name __asm__("$20");
    if (func_8001CE10() < 0) return -2;
    i = 0;
    entry = D_800A6D78;
    __asm__ volatile ("" : "+r" (entry));
    while (i < 4) {
        if (!entry->state) break;
        if ((entry->state & 255) < 0 || (entry->state & 255) >= 4) return -23;
        i++;
        entry++;
    }
    if (i == 4) { three = 3; __asm__ volatile ("" : : "r" (three)); return -10; }
    __asm__ volatile ("" : "=r" (three));
    if (arg1 < 0) arg1 = -arg1;
    __asm__ volatile ("" : "+r" (arg1));
    if ((arg1 & 15) == three) arg1 = 1;
    mode = arg1;
    if ((arg1 & 15) == 2) { arg1 = 1; mode = arg1; }
    __asm__ volatile ("" : "+r" (arg1));
    argument = arg0;
    __asm__ volatile ("" : "+r" (argument));
    name = (char *)0x800a0000;
    __asm__ volatile ("" : "+r" (name));
    ret = func_8001DC48(mode, argument, arg2, name + 0x6dd8);
    if (ret < 0) return ret;
    if (arg1 != 1 && arg1 != 17) return -6;
    ret = func_8001C478(arg1);
    if (ret < 0) return ret;
    j = 0;
    do {
        if (func_8008FF20(D_800A6E20, name + 0x6dd8)) break;
        j++;
    } while (j < 5);
    if (j != 5) {
        register s32 value __asm__("$2");
        register s32 copy __asm__("$4");
        D_800A6D78[i].pos = func_8001C588(D_800A6E20);
        value = D_800A6E20[0].value;
        copy = D_800A6D78[i].pos;
        D_800A6D78[i].mode = arg1;
        D_800A6D78[i].value = value;
        D_800A6D78[i].copy = copy;
        {
            register s32 state __asm__("$2");
            state = 1;
            D_800A6D78[i].state = state;
        }
        __asm__ volatile ("" : : "r" (copy));
        return i;
    }
    __asm__ volatile ("");
    return -5;
}
