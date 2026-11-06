typedef unsigned char u8;
typedef unsigned int u32;

struct Packet {
    u8 pad[3];
    u8 length;
    u32 command;
    u32 data;
};

void func_80093A74(struct Packet *packet, int enabled)
{
    packet->length = 2;
    if (enabled)
        packet->command = 0xE6000001;
    else
        packet->command = 0xE6000000;
    packet->data = 0;
}
