// Categories: Platform > Dispatch
extern unsigned char D_80010000[];
extern void (*D_8009DDEC)(void *, unsigned int, unsigned int);

/**
 *  @brief Dispatches a split 16-bit value to a stored function.
 *
 *  Passes a fixed address and two fields derived from value to the
 *  function pointer, with the low six bits shifted left by four.
 *  @param value Packed value split into low six bits and upper ten bits.
 *  @par Globals
 *  D_80010000 supplies the base address; D_8009DDEC holds the function pointer.
 *  @par Control flow
 *  Computes the arguments and makes one indirect call.
 *  @par Calls
 *  The D_8009DDEC function receives the base plus 0x28b4 and both fields.
 *  @par Side effects
 *  The indirect call may have side effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800922E4(unsigned short value)
{
    unsigned int low = value & 0x3f;
    unsigned int high = value;
    unsigned char *address;
    __asm__("" : : "r"(low), "r"(high));
    address = D_80010000;
    __asm__("" : "=r"(address) : "0"(address));
    D_8009DDEC(address + 0x28b4, low << 4, high >> 6);
}
