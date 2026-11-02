#include "game/types.h"
extern u8 *D_8009AB08;
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_8001E318(void *destination, void *source, int count);
extern s32 func_8008CCC0(char *text);
void func_80020F34(char *t, s32 index)
{
    register char *text __asm__("$17") = t;
    u8 *packet; register s32 off __asm__("$2");
    s32 length; register u32 i __asm__("$4"); register u32 checksum __asm__("$5"); register u32 tt __asm__("$3"); register u32 b __asm__("$2");
    __asm__("" : "=r"(text) : "0"(text));
    off = index * 0x80; packet = D_8009AB08; packet += 0x9B0; packet += off;
    func_8001E4C0(packet, 0, 0x80);
    packet[0] = 0x51;
    *(s32 *)(packet + 4) = 0x2000;
    *(u16 *)(packet + 8) = 0xFFFF;
    length = func_8008CCC0(text);
    func_8001E318(packet + 0xA, text, length);
    checksum = 0;
    for (i = 0; i < 0x7F; i++) { b = *packet++; tt = checksum ^ b; checksum = tt; }
    *packet = tt;
}
