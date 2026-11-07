typedef signed char s8;
typedef unsigned char u8;

struct Entry {
    unsigned int unknown;
    u8 *values;
    u8 unused;
    u8 replacement;
    u8 limit;
    u8 pad[9];
};
extern struct Entry D_8009C0A8[3];
extern s8 *D_8009F468;

void func_8007D2DC(int mode)
{
    int i, j;
    struct Entry *p;
    struct Entry *base;
    u8 *value;
    i = 0;
    base = D_8009C0A8;
    for (; i < 2; i++) {
        for (j = 0, p = base; j < 3; j++, p++) {
            __asm__("" : "+r"(p));
            value = p->values + i;
            if (*value > p->limit) {
                if (mode == 0) {
                    if (i == 0 && j == D_8009F468[0] - 1)
                        continue;
                    if (i == 1 && j == D_8009F468[1] - 5)
                        continue;
                }
                *value = p->replacement;
            }
        }
    }
}
