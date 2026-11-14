typedef unsigned int u32;

extern u32 D_8009C740;
extern u32 D_8009C7A8;
extern int func_8008A9A8(u32 arg, int argvalue);

void func_8008B7D0(u32 arg)
{
    u32 r;
    r = ((u32 (*)(u32))D_8009C740)(arg);
    D_8009C7A8 = r;
    func_8008A9A8(arg, -2);
}
