// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"
extern u8 *gOperationState __asm__("D_8009AB08");
extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern void gCopyMemory(void *destination, void *source, int count) __asm__("func_8001E318");
extern s32 gBiosStrlen(char *text) __asm__("func_8008CCC0");

void gBuildCardFileDirectoryFrame(char *t, s32 index) __asm__("func_80020F34");

/**
 * @brief Builds a 128-byte directory frame for a new memory-card file.
 *
 * @param t Filename copied into the directory frame.
 * @param index Directory frame index.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears the selected frame at manager +0x9B0 plus index times 128. Writes
 * type 0x51, size 0x2000, and link 0xFFFF, copies the supplied name to +0x0A,
 * and stores the XOR of bytes 0 through 126 at +0x7F.
 *
 * @par Globals
 * Reads gOperationState and writes the selected directory frame through it.
 *
 * @par Callers
 * Direct calls in the retail listing are from gQueueCardFileWrite.
 *
 * @par Role and path
 * The card write path builds this frame before staging creation of a new save.
 *
 * @par Unknown
 * The filename length is accepted from the string-length helper.
 */
void gBuildCardFileDirectoryFrame(char *t, s32 index)
{
    register char *text __asm__("$17") = t;
    u8 *packet; register s32 off __asm__("$2");
    s32 length; register u32 i __asm__("$4"); register u32 checksum __asm__("$5"); register u32 tt __asm__("$3"); register u32 b __asm__("$2");
    __asm__("" : "=r"(text) : "0"(text));
    off = index * 0x80; packet = gOperationState; packet += 0x9B0; packet += off;
    gFillMemory(packet, 0, 0x80);
    packet[0] = 0x51;
    *(s32 *)(packet + 4) = 0x2000;
    *(u16 *)(packet + 8) = 0xFFFF;
    length = gBiosStrlen(text);
    gCopyMemory(packet + 0xA, text, length);
    checksum = 0;
    for (i = 0; i < 0x7F; i++) { b = *packet++; tt = checksum ^ b; checksum = tt; }
    *packet = tt;
}
