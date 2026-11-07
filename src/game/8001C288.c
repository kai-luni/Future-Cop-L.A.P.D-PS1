typedef unsigned char u8;
void func_8001C288(u8 *p)
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
