// Categories: Graphics > GPU Command Lists > Draw Mode | Platform > GPU
typedef unsigned char u8;
typedef unsigned int u32;

struct Packet {
    u8 pad[3];
    u8 length;
    u32 command;
    u32 data;
};
void gSetMaskBitPacket(struct Packet *packet, int enabled) __asm__("func_80093A74");


/**
 * @brief Builds a minimal "mask bit setting" primitive (GP0 0xE6).
 *
 * @param packet Target packet; length = 2.
 * @param enabled Non-zero writes 0xE6000001 ("set mask bit when drawing"),
 *                zero writes 0xE6000000.
 * @return Nothing; the data word is 0.
 *
 * @par Difference to func_80093A4C
 * Both fields are controlled here by a single switch; func_80093A4C exposes
 * them separately.
 *
 * @par Environment path
 * Builds the GP0 E6 mask setting packet: bit 0 (force the mask bit on drawn
 * pixels) follows the flag, bit 1 (check mask before drawing) stays 0, and
 * the trailing word is 0. No caller references it in the sources.
 */
void gSetMaskBitPacket(struct Packet *packet, int enabled)
{
    packet->length = 2;
    if (enabled)
        packet->command = 0xE6000001;
    else
        packet->command = 0xE6000000;
    packet->data = 0;
}
