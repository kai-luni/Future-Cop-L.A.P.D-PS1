// Categories: Platform > Block Transfer
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

extern State *gOperationState __asm__("D_8009AB08");
extern void func_8008D640(int);
extern void func_8008D670(int, int, void *);
extern void func_8008D680(int, int, void *);
extern void func_8001E318(void *, void *, int);

/**
 *  @brief Advances a mode-driven block transfer and handles completion or errors.
 *
 *  Uses the shared transfer state to receive or send 128-byte blocks, optionally
 *  reserving two words for a sequence number and checksum.
 *  @return Always zero.
 *  @par Globals
 *  D_8009AB08 points to the transfer state, buffer, counters, and handle.
 *  @par Control flow
 *  Dispatches on mode; checksum loops cover words 2 through 31. Receive errors
 *  consume retries before stopping; modes 3 and 6 advance outgoing blocks.
 *  @par Calls
 *  func_8001E318 copies block data; func_8008D680 requests the next block;
 *  func_8008D670 submits a block; func_8008D640 stops the handle.
 *  @par Side effects
 *  Updates transfer state and buffers, and starts or stops handle operations.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_8001F394(void)
{
    State *p;
    State *r;
    unsigned int sum;
    unsigned int sum2;
    unsigned int i;
    int n;

    gOperationState->f1BB0 = 0;
    gOperationState->f9A8 = 1;
    switch ((unsigned int)gOperationState->mode) {
    case 1:
        return 0;
    case 2:
        gOperationState->f994++;
        if (gOperationState->f994 == 1)
            return 0;
        sum = 0;
        for (i = 2; i < 32; i++)
            sum += (gOperationState->arr[i] ^ gOperationState->arr[32 - i]) + i;
        p = gOperationState;
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
            r = gOperationState;
            if (r->f90C) {
                func_8001E318((void *)r->ptr, (void *)&r->arr[2], 0x78);
                r = gOperationState;
                r->ptr = r->ptr + 0x78;
            } else {
                func_8001E318((void *)r->ptr, (void *)&r->arr[0], 0x80);
                r = gOperationState;
                r->ptr = r->ptr + 0x80;
            }
            p = gOperationState;
            p->f9A0 = 3;
            p->v998++;
            p->f99C--;
            if (p->f99C <= 0) {
                if (p->f8F8 == 1)
                    p->f8F8 = 0;
                gOperationState->f900 = gOperationState->h;
                gOperationState->mode = 0;
                func_8008D640(gOperationState->h);
                return 0;
            }
        }
        func_8008D680(p->h, p->v998, (void *)&p->arr[0]);
        return 0;
    case 3:
    case 6:
        sum2 = 0;
        gOperationState->f994++;
        if (gOperationState->f994 == 1)
            return 0;
        gOperationState->f99C--;
        if (gOperationState->f99C <= 0) {
            if (gOperationState->mode == 6) {
                gOperationState->v998 = gOperationState->f1BB4;
                gOperationState->f99C = 1;
                n = gOperationState->f1BB4;
                gOperationState->f90C = 0;
                gOperationState->f994 = 0;
                gOperationState->f9A4 = 0;
                gOperationState->f8F8 = 3;
                gOperationState->ptr = (char *)gOperationState + ((n << 7) + 0x9B0);
                goto reset;
            }
            gOperationState->f900 = gOperationState->h;
            goto reset;
        }
        gOperationState->f9A0 = 3;
        gOperationState->v998++;
        if (gOperationState->f90C) {
            gOperationState->ptr = gOperationState->ptr + 0x78;
            func_8001E318((void *)&gOperationState->arr[2], (void *)gOperationState->ptr, 0x78);
            for (i = 2; i < 32; i++)
                sum2 += (gOperationState->arr[i] ^ gOperationState->arr[32 - i]) + i;
            gOperationState->arr[0] = gOperationState->v998;
            gOperationState->arr[1] = sum2;
        } else {
            gOperationState->ptr = gOperationState->ptr + 0x80;
            func_8001E318((void *)&gOperationState->arr[0], (void *)gOperationState->ptr, 0x80);
        }
        func_8008D670(gOperationState->h, gOperationState->v998, (void *)&gOperationState->arr[0]);
        return 0;
    default:
    reset:
        gOperationState->mode = 0;
        func_8008D640(gOperationState->h);
        return 0;
    }
}
