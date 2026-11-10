typedef unsigned char u8;
typedef unsigned int u32;

struct Packet {
    u8 pad[3];
    u8 length;
    u32 command;
};

void func_80092634(struct Packet *packet, int dfe, int dtd, u32 tpage)
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
