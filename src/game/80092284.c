// Categories: Graphics > Packed Parameters
extern  void (*gLogPrintf)(char *, int, int, int, int) __asm__("D_8009DDEC");

/**
 *  @brief Dispatches fields extracted from a packed 16-bit value.
 *
 *  Splits the value into four masked arguments and passes a fixed address
 *  to a callback; the meaning of the packed fields is not established here.
 *  @param value Packed input from which the callback arguments are derived.
 *  @par Globals
 *  D_8001289C supplies the fixed address; D_8009DDEC supplies the callback.
 *  @par Control flow
 *  Computes the masked fields and makes one unconditional indirect call.
 *  @par Calls
 *  The D_8009DDEC callback receives the address and four extracted values.
 *  @par Side effects
 *  The callback may modify state; this function makes no direct global writes.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80092284(unsigned short value)
{
    unsigned int arg1 = value >> 7;
    unsigned int arg2 = value >> 5;
    unsigned int arg3 = value << 6;
    gLogPrintf(({ char *address;
                   __asm__("lui %0,%%hi(D_8001289C)\n\taddiu %0,%0,%%lo(D_8001289C)" : "=r" (address) : "r" (arg1), "r" (arg2), "r" (arg3));
                   address; }), arg1 & 3, arg2 & 3,
               arg3 & 0x7C0, ((value << 4) & 0x100) | ((value >> 2) & 0x200));
}
