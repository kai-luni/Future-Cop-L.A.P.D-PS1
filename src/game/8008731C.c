typedef unsigned char u8;
typedef unsigned int u32;

void func_8008731C(u8 *dst, u8 *src, u32 count)
{
    u32 i;
    for (i = 0; i < count; i += 15) {
        *dst++ = *src++;
        *dst++ = 0;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}
