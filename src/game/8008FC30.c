// Categories: Platform > Hardware Transfer
typedef unsigned char u8;
typedef unsigned int u32;
extern volatile u8 *D_8009CB6C;
extern volatile u8 *D_8009CB78;
extern volatile u32 *D_8009CBA0;
extern volatile u32 *D_8009CB7C;
extern volatile u32 *D_8009CBA4;
extern volatile u32 *D_8009CBA8;
extern volatile u32 *D_8009CBAC;
extern volatile u32 *D_8009CBB0;

/**
 *  @brief Configures and waits for a hardware transfer to a destination.
 *
 *  The routine writes a fixed register sequence, waits for readiness, starts
 *  the transfer, and waits for its control bit to clear.
 *  @param destination Destination value written to the transfer register.
 *  @param size Transfer size combined with the 0x10000 control bit.
 *  @return Always 0 after the transfer control bit clears.
 *  @par Globals
 *  D_8009CB6C, D_8009CB78, D_8009CBA0, D_8009CB7C, D_8009CBA4,
 *  D_8009CBA8, D_8009CBAC, and D_8009CBB0 point to hardware registers.
 *  @par Control flow
 *  Two unbounded polling loops wait for readiness and transfer completion.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes hardware registers and initiates a transfer; polling can block indefinitely.
 *  @par Callers
 *  Confirmed: func_8008E598.
 */
int func_8008FC30(u32 destination, u32 size)
{
    volatile u8 *p;
    *D_8009CB6C = 0;
    *D_8009CB78 = 0x80;
    *D_8009CBA0 = 0x20943;
    *D_8009CB7C = 0x1323;
    *D_8009CBA4 |= 0x8000;
    *D_8009CBA8 = destination;
    *D_8009CBAC = size | 0x10000;
    p = D_8009CB6C;
    __asm__ volatile("nop" : "+r"(p));
    while ((*p & 0x40) == 0) {}
    *D_8009CBB0 = 0x11000000;
    while (*D_8009CBB0 & 0x1000000) {}
    *D_8009CB7C = 0x1325;
    return 0;
}
