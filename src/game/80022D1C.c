// Categories: Game Engine > State Flags
typedef unsigned char u8;
extern int gMode1IndexBase __asm__("D_800A09D4");

/**
 *  @brief Updates a byte in a data buffer according to a mode.
 *
 *  Modes zero and one set bits, while mode three writes a whole byte.
 *  The buffer's purpose and the meaning of these flags are not established.
 *  @param data Buffer to modify.
 *  @param mode Selects the update; other values leave the buffer unchanged.
 *  @param offset Offset used by mode three, before subtracting 15.
 *  @par Globals
 *  D_800A09D4 supplies the index basis for mode one.
 *  @par Control flow
 *  Branches on modes 0, 1, and 3; all other modes return without a write.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies one buffer byte for a supported mode, without bounds checks.
 *  @par Callers
 *  Confirmed: func_80022D84.
 */
void func_80022D1C(u8 *data, int mode, int offset)
{
    register int index __asm__("$2");
    register u8 *ptr __asm__("$3");
    if (mode == 1) goto one;
    if (mode == 0) goto zero;
    if (mode == 3) goto three;
    goto done;
zero:
    data[1] |= 4;
    goto done;
one:
    index = gMode1IndexBase - 15;
    __asm__("" : "=r"(index) : "0"(index));
    ptr = data + index;
    __asm__("" : "=r"(ptr) : "0"(ptr));
    *ptr |= 1;
    goto done;
three:
    data[offset - 15] = 1;
done:
    return;
}
