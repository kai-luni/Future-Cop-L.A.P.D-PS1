typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u8 pad[3];
    u8 length;
    u32 command;
    u32 data;
} Packet;

extern u32 func_80093F58(int, int, u32);
extern u32 func_800940C4(int);

void func_80093A9C(Packet *packet, int arg1, int arg2, u16 arg3, int arg4)
{
    packet->length = 2;
    packet->command = func_80093F58(arg1, arg2, arg3);
    packet->data = func_800940C4(arg4);
}
