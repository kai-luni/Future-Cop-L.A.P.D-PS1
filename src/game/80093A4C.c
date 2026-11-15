// Categories: Graphics > GPU Command Lists > Draw Mode | Platform > GPU
struct Packet {
    unsigned char pad[3];
    unsigned char length;
    unsigned int command;
    unsigned int unused;
};

/**
 * @brief Builds a "mask bit setting" primitive (GP0 0xE6) with both fields.
 *
 * @param packet Target packet; length is set to 2 (command word + one data
 *               word).
 * @param flag1 Non-zero sets bit 1 of the command ("do not draw pixels whose
 *              mask bit is already set", i.e. force-set behaviour off).
 * @param flag2 Non-zero sets bit 0 ("set the mask bit when drawing").
 * @return Nothing; the packet holds 0xE6xxxxxx and a zero data word.
 *
 * @par Confirmed
 * command = 0xE6000000 | (flag1 ? 2 : 0) | (flag2 != 0).
 *
 * @par Callers
 * None in the symbol table - the packet is emitted inline by the effect
 * renderers.
 */
void func_80093A4C(struct Packet *packet, int flag1, int flag2)
{
    unsigned int command;
    packet->length = 2;
    command = 0xE6000000;
    if (flag1)
        command |= 2;
    packet->command = command | (flag2 != 0);
    packet->unused = 0;
}
