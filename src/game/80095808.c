extern void func_80094CEC(void);
extern void func_800914E0(int, void (*)(void));

void func_80095808(void)
{
    register void (*callback)(void) asm("$5");
    asm("lui %0, %%hi(func_80094CEC)\n\taddiu %0, %0, %%lo(func_80094CEC)" : "=r"(callback));
    func_800914E0(2, callback);
}
