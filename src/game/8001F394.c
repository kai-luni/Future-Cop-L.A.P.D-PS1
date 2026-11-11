typedef struct State {
    char pad0[0x8F4];
    volatile int mode;
    volatile int f8F8;
    volatile int h;
    volatile int f900;
    char pad904[8];
    volatile int f90C;
    volatile char *ptr;
    volatile unsigned int arr[0x20];
    volatile int f994;
    volatile int v998;
    volatile int f99C;
    volatile int f9A0;
    volatile int f9A4;
    volatile int f9A8;
    char pad9AC[0x1200 + 4];
    volatile int f1BB0;
    volatile int f1BB4;
} State;

extern State *D_8009AB08;
extern void func_8008D640(int);
extern void func_8008D670(int, int, void *);
extern void func_8008D680(int, int, void *);
extern void func_8001E318(void *, void *, int);

int func_8001F394(void)
{
    State *p;
    State *r;
    unsigned int sum;
    unsigned int sum2;
    unsigned int i;
    int n;

    D_8009AB08->f1BB0 = 0;
    D_8009AB08->f9A8 = 1;
    switch ((unsigned int)D_8009AB08->mode) {
    case 1:
        return 0;
    case 2:
        D_8009AB08->f994++;
        if (D_8009AB08->f994 == 1)
            return 0;
        sum = 0;
        for (i = 2; i < 32; i++)
            sum += (D_8009AB08->arr[i] ^ D_8009AB08->arr[32 - i]) + i;
        p = D_8009AB08;
        if (p->f90C != 0 && (p->arr[0] != p->v998 || p->arr[1] != sum)) {
            if (p->f9A0 > 0) {
                p->f9A0--;
            } else {
                p->f9A4 = 1;
                p->mode = 0;
                func_8008D640(p->h);
                return 0;
            }
        } else {
            r = D_8009AB08;
            if (r->f90C) {
                func_8001E318((void *)r->ptr, (void *)&r->arr[2], 0x78);
                r = D_8009AB08;
                r->ptr = r->ptr + 0x78;
            } else {
                func_8001E318((void *)r->ptr, (void *)&r->arr[0], 0x80);
                r = D_8009AB08;
                r->ptr = r->ptr + 0x80;
            }
            p = D_8009AB08;
            p->f9A0 = 3;
            p->v998++;
            p->f99C--;
            if (p->f99C <= 0) {
                if (p->f8F8 == 1)
                    p->f8F8 = 0;
                D_8009AB08->f900 = D_8009AB08->h;
                D_8009AB08->mode = 0;
                func_8008D640(D_8009AB08->h);
                return 0;
            }
        }
        func_8008D680(p->h, p->v998, (void *)&p->arr[0]);
        return 0;
    case 3:
    case 6:
        sum2 = 0;
        D_8009AB08->f994++;
        if (D_8009AB08->f994 == 1)
            return 0;
        D_8009AB08->f99C--;
        if (D_8009AB08->f99C <= 0) {
            if (D_8009AB08->mode == 6) {
                D_8009AB08->v998 = D_8009AB08->f1BB4;
                D_8009AB08->f99C = 1;
                n = D_8009AB08->f1BB4;
                D_8009AB08->f90C = 0;
                D_8009AB08->f994 = 0;
                D_8009AB08->f9A4 = 0;
                D_8009AB08->f8F8 = 3;
                D_8009AB08->ptr = (char *)D_8009AB08 + ((n << 7) + 0x9B0);
                goto reset;
            }
            D_8009AB08->f900 = D_8009AB08->h;
            goto reset;
        }
        D_8009AB08->f9A0 = 3;
        D_8009AB08->v998++;
        if (D_8009AB08->f90C) {
            D_8009AB08->ptr = D_8009AB08->ptr + 0x78;
            func_8001E318((void *)&D_8009AB08->arr[2], (void *)D_8009AB08->ptr, 0x78);
            for (i = 2; i < 32; i++)
                sum2 += (D_8009AB08->arr[i] ^ D_8009AB08->arr[32 - i]) + i;
            D_8009AB08->arr[0] = D_8009AB08->v998;
            D_8009AB08->arr[1] = sum2;
        } else {
            D_8009AB08->ptr = D_8009AB08->ptr + 0x80;
            func_8001E318((void *)&D_8009AB08->arr[0], (void *)D_8009AB08->ptr, 0x80);
        }
        func_8008D670(D_8009AB08->h, D_8009AB08->v998, (void *)&D_8009AB08->arr[0]);
        return 0;
    default:
    reset:
        D_8009AB08->mode = 0;
        func_8008D640(D_8009AB08->h);
        return 0;
    }
}
