// Categories: Utilities > BCD Arithmetic
typedef unsigned char u8;
void gIncrementBcdLoc(u8 *p) __asm__("func_8001C288");


/**
 *  @brief Increments a three-byte packed-BCD position in place.
 *
 *  The bytes are decoded as decimal fields, incremented with carries at
 *  75 and 60, and encoded back into packed BCD.
 *  The first field receives the final carry without an explicit rollover.
 *  @param p Pointer to the three packed-BCD bytes to update.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Two equality checks propagate carries from the third and second fields.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites all three bytes at p; input digits are not validated.
 *  @par Callers
 *  Confirmed: func_8001C808.
 *
 * @par Role and path
 * Pure BCD arithmetic helper that decodes three packed-BCD bytes (minute,
 * second, sector), increments the sector byte with carries at 75 and 60, and
 * re-encodes them in place. Called by func_8001C808 to advance gCdCurrentLoc
 * by one CD sector; touches no globals directly.
 */
void gIncrementBcdLoc(u8 *p)
{
    volatile u8 a[3];
    u8 x;
    register unsigned int y __asm__("$5");
    register unsigned int q __asm__("$4");
    register int carry __asm__("$3");
    a[0] = (p[0] >> 4) * 10 + (p[0] & 15);
    a[1] = (p[1] >> 4) * 10 + (p[1] & 15);
    x = (p[2] >> 4) * 10 + (p[2] & 15) + 1;
    a[2] = x;
    if (x == 75) { a[2] = 0; carry = 1; } else { carry = 0; }
    x = a[1] + carry;
    a[1] = x;
    if (x == 60) { a[1] = 0; carry = 1; } else { carry = 0; }
    a[0] += carry;
    y = a[0]; __asm__ volatile ("" : "+r" (y)); q = (u8)(y / 10); __asm__ volatile ("" : "+r" (q)); p[0] = (q << 4) + y % 10;
    y = a[1]; __asm__ volatile ("" : "+r" (y)); q = (u8)(y / 10); __asm__ volatile ("" : "+r" (q)); p[1] = (q << 4) + y % 10;
    y = a[2]; __asm__ volatile ("" : "+r" (y)); q = (u8)(y / 10); __asm__ volatile ("" : "+r" (q)); p[2] = (q << 4) + y % 10;
}
