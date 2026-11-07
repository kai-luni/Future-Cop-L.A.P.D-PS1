extern int D_8009F278;
extern int D_8009F9BC;
extern volatile int D_8009F650;
extern void D_80023C94(void);
struct Request { int address; unsigned int size; void *data; void (*callback)(void); int unused; int mode; };
extern void func_800212F8(struct Request *);

int func_80023CA4(void *data, unsigned int size, int mode)
{
    struct Request req;
    int result = 0;
    int timeout = 0xfffff;
    if (D_8009F278) {
        if (mode == 1) D_8009F9BC = D_8009F278;
        size = (size + 63) >> 6 << 6;
        req.mode = 1;
        req.address = D_8009F9BC;
        req.size = size;
        req.data = data;
        req.callback = D_80023C94;
        D_8009F650 = 0;
        func_800212F8(&req);
        while (!D_8009F650 && timeout-- > 0) {}
        if (timeout > 0) {
            D_8009F9BC += size;
            result = 1;
        }
    }
    return result;
}
