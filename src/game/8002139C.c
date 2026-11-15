typedef struct Request {
    int a, b, c;
    void (*callback)(int);
    int context, mode;
} Request;
extern Request D_800A8EA8[];
extern int D_8009F998, D_8009FA80, D_8009FAD0, D_8009FA34;
extern void func_80021490(Request *);

void func_8002139C(void)
{
    Request r;
    if (D_8009FAD0)
        return;
    if (D_8009FA80 == D_8009F998)
        return;
    r = D_800A8EA8[D_8009FA80];
    D_8009FAD0 = 1;
    if (r.callback)
        r.callback(r.context);
    D_8009FA80++;
    if (D_8009FA80 >= 9)
        D_8009FA80 = 0;
    if (D_8009F998 != D_8009FA80)
        func_80021490(&D_800A8EA8[D_8009FA80]);
    else
        D_8009FA34 = 0;
    D_8009FAD0 = 0;
}
