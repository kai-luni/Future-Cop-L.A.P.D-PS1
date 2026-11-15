// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"
extern u8 *D_8009AB08;
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_8001E318(void *destination, void *source, int count);
extern s32 func_8008CCC0(char *text);
/**
 * @brief Builds a 0x80-byte memory-card name packet with header, title and
 *        XOR checksum.
 *
 * @param t Title string.
 * @param index Slot index (packet offset index * 0x80).
 * @return Nothing.
 *
 * @par Confirmed
 * The packet lives at D_8009AB08 + 0x9B0 + index * 0x80, zeroed first.
 * Header: byte 0 = 0x51, +4 = 0x2000, +8 = 0xFFFF. The title (BIOS strlen
 * func_8008CCC0, copied via func_8001E318) starts at +0xA. The XOR of the
 * first 0x7F bytes is stored as byte 0x7F.
 *
 * @par Likely role
 * The save-icon/title packet writer behind func_80020FDC: 0x51 is the
 * card-format magic, 0x2000 likely the icon size/offset word, the checksum
 * protects the directory entry.
 *
 * @par Unknown
 * The exact meaning of the 0x2000/0xFFFF header words.
 *
 * @par Callers
 * func_80020FDC.
 */
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
