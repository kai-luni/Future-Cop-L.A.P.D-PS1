// Categories: Graphics > Packet Setup
/**
 *  @brief Sets two bytes in a caller-provided buffer.
 *
 *  Writes fixed values at offsets 3 and 7, leaving the other bytes
 *  untouched. The source does not identify the buffer's format.
 *  @param p Buffer with writable bytes at offsets 3 and 7.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs both stores unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Sets p[3] to 4 and p[7] to 0x20.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80092460(unsigned char *p)
{
    p[3] = 4;
    p[7] = 0x20;
}
