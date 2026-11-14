/* Tail jump to the fixed address 0xA000DFAC. A computed goto materialises
 * the address in $v0 and jumps without a call frame; naming the symbol
 * (link path) keeps the %hi/%lo pair as lui+addiu instead of lui+ori. */
extern char D_A000DFAC[];

void func_8008DCC0(void)
{
    goto *(void *)D_A000DFAC;
}
