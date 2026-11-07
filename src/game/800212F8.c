typedef struct Request {
    int a, b, c;
    void (*callback)(int);
    int context, mode;
} Request;
extern Request D_800A8EA8[];
extern int D_8009F998, D_8009FA80, D_8009FAD0, D_8009FA34;
extern void func_80021490(Request *);

void func_800212F8(Request *req)
{
    register Request *dst __asm__("$2") = &D_800A8EA8[D_8009F998];
    dst->a = req->a;
    dst->b = req->b;
    dst->c = req->c;
    dst->callback = req->callback;
    dst->context = req->context;
    dst->mode = req->mode;
    if (D_8009F998 == D_8009FA80)
        func_80021490(req);
    D_8009F998++;
    if (D_8009F998 >= 9)
        D_8009F998 = 0;
}

