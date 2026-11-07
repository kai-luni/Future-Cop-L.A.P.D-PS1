typedef unsigned int u32;
typedef unsigned char u8;
extern u32 D_8009F528;
extern u8 *D_8009F52C, *D_8009F530, *D_8009F534, *D_8009F538, *D_8009F53C, *D_8009F540;
extern u8 *D_8009F544, *D_8009F548, *D_8009F54C, *D_8009F550;
extern u8 **D_8009F554, **D_8009F558, **D_8009F55C, **D_8009F560;
void func_80012E48(u8 *base, u32 size)
{
    u8 *p = base;
    u32 tag, length, index;
    u8 *data;
    D_8009F544 = 0;
    D_8009F548 = 0;
    D_8009F54C = 0;
    D_8009F550 = 0;
    if (size) do {
        tag = *(u32 *)p;
        length = *(u32 *)(p + 4);
        index = *(u32 *)(p + 8);
        data = p + 12;
        switch (tag) {
        case 0x3444564c: D_8009F554[index] = data; break;
        case 0x34444e4c: D_8009F558[index] = data; break;
        case 0x3344524c: D_8009F55C[index] = data; break;
        case 0x33445246: D_8009F560[index] = data; break;
        case 0x33444242: D_8009F540 = p + 8; break;
        case 0x34444749: D_8009F52C = p; break;
        case 0x3344514c: D_8009F530 = data; break;
        case 0x3344544c: D_8009F534 = data; D_8009F528 = length - 12; break;
        case 0x33444859: D_8009F538 = data; break;
        case 0x33444d49: D_8009F53C = data; break;
        case 0x416e6d44: *(u32 *)(p + 8) = (length - 12) >> 4; D_8009F544 = p + 8; break;
        case 0x33444853: D_8009F548 = p + 8; break;
        case 0x33445441: D_8009F54C = p; break;
        case 0x3344414c: D_8009F550 = p; break;
        }
        p += length;
    } while ((u32)(p - base) < size);
}
