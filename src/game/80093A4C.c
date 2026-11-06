struct Packet {
    unsigned char pad[3];
    unsigned char length;
    unsigned int command;
    unsigned int unused;
};

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
