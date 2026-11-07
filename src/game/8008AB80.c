#include "game/types.h"

struct Port { u8 pad0[0x3c]; u8 *response; u8 pad40[4]; u8 index; u8 count; u8 pad46[0xa2]; u8 id; };
struct Device { volatile u8 data; u8 pad1[3]; volatile u16 status; u8 pad6[8]; volatile u16 control; };
extern struct Device *D_8009C78C;
extern volatile u32 *D_8009C788;
extern u32 D_8009C76C;
extern u32 D_800A6E18, D_800A9448;
extern void func_8008C890(u32);
extern s32 func_8008C8B0(void);

s32 func_8008AB80(struct Port *port, s32 value_)
{
    register s32 control __asm__("$17");
    register s32 received __asm__("$18");
    register s32 value __asm__("$19");
    u32 count;
    register s32 data __asm__("$2");
    register s32 index __asm__("$3");
    register s32 id __asm__("$2");
    struct Device *device;
    value = value_;
    __asm__("" : "+r"(value));
    control = 0x88;
    id = port->response[0];
    if ((id >> 4) == 8 && port->index >= 9)
        control = 0x22;
    {
        register struct Device *poll __asm__("$3");
        poll = D_8009C78C;
        __asm__ volatile ("nop" : "+r"(poll));
        while (!(poll->status & 2)) {}
    }
    func_8008C890(400);
    device = D_8009C78C;
    data = device->data;
    index = *(volatile u8 *)&port->index;
    __asm__("" : "+r"(data), "+r"(index));
    received = data & 0xff;
    if (index != 0) goto normal;
    if ((received >> 4) == 8) goto special;
normal:
    device->control = control;
    goto done;
special:
    device->control = 0x22;
done:;
    while (!(*D_8009C788 & 0x80)) {
        count = *(volatile u16 *)0x1f801120 & 0xffff;
        if (count < D_800A6E18) {
            if (*(volatile u16 *)0x1f801128)
                count += *(volatile u16 *)0x1f801128;
            else
                count += 0x10000;
        }
        if (*(volatile u16 *)0x1f801124 & 0x200) {
            if (count - D_800A6E18 >= D_800A9448) return -2;
        }
        if ((count - D_800A6E18) >> 3 >= D_800A9448) return -2;
    }
    if (port->id != 8 && D_8009C76C == 2) {
        func_8008C890(60);
        while (!func_8008C8B0()) {}
    }
    D_8009C78C->data = value;
    port->count++;
    if (*(volatile u8 *)&port->index != 0xff) {
        register u8 *response __asm__("$2");
        index = *(volatile u8 *)&port->index;
        response = port->response;
        __asm__("" : "+r"(index), "+r"(response));
        response[index] = received;
    }
    port->index++;
    return received;
}
