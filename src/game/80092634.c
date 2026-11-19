// Categories: Graphics > GPU Command Lists > Draw Mode | Platform > GPU
typedef unsigned char u8;
typedef unsigned int u32;

struct Packet {
    u8 pad[3];
    u8 length;
    u32 command;
};
void gSetDrawModePacket(struct Packet *packet, int dfe, int dtd, u32 tpage) __asm__("func_80092634");


/**
 * @brief Builds a draw-mode/texpage primitive (GP0 0xE1).
 *
 * @param packet Target packet (length = 1: command only).
 * @param dfe Non-zero sets the dither bit (0x400 in the data word).
 * @param dtd Non-zero sets the "draw to display area" bit (0x200).
 * @param tpage Texture page word; only the low 9 bits plus bit 9 (semi
 *              transparency / texture page base) survive - masked with 0x9FF.
 * @return Nothing.
 *
 * @par Confirmed
 * command = 0xE1000000 | (dtd ? 0x200 : 0) | ((tpage & 0x9FF) | (dfe ? 0x400 : 0)).
 *
 * @par Callers
 * None in the symbol table; emitted inline by the sprite and model paths.
 *
 * @par Environment path
 * Builds a draw-mode packet: tpage and 0x9FF keeps page X/Y, blend bits 5-6,
 * colour depth bits 7-8 and texture-disable bit 11; dtd non-zero adds 0x200
 * (dither) and dfe non-zero adds 0x400 (drawing to the display area allowed).
 * No caller references it in the sources.
 */
void gSetDrawModePacket(struct Packet *packet, int dfe, int dtd, u32 tpage)
{
    register u32 command asm("$3");
    register u32 bits asm("$2");
    packet->length = 1;
    command = 0xE1000000;
    if (dtd)
        command |= 0x200;
    bits = tpage & 0x9FF;
    if (dfe)
        bits |= 0x400;
    packet->command = command | bits;
}
