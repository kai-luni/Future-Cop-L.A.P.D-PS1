typedef struct { int pad[3]; int size; } Input;
typedef struct { int handle; int size; int arg; void (*callback)(); int context; int flag; } Request;
extern int func_80097B28(int);
extern void func_800212F8(Request *);
extern void func_80021888(void);
extern volatile int D_8009F628;
int func_80021898(Input *input, int arg, void (*callback)(), int context)
{
    int size = input->size;
    int result = 0;
    int timeout = 0xFFFFF;
    int handle;
    Request req;
    handle = func_80097B28(((size + 63) >> 6) << 6);
    if (handle < 0) {
        result = -1;
    } else {
        req.handle = handle;
        req.size = size;
        req.arg = arg;
        req.flag = 1;
        if (callback) {
            req.callback = callback;
            req.context = context;
        } else {
            req.callback = func_80021888;
            D_8009F628 = 0;
        }
        func_800212F8(&req);
        if (!callback) {
            while (!D_8009F628) {
                if (timeout-- <= 0)
                    return result;
            }
        }
        result = handle;
    }
    return result;
}
