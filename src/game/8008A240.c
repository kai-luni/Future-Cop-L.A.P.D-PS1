extern void *(*D_8009C73C)(int);
extern void func_8008AE70(void *, int, int);

void func_8008A240(int arg0, int arg1, int arg2)
{
    func_8008AE70(D_8009C73C(arg0), arg1, arg2);
}
