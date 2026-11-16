// Categories: Platform > Controller Input | Platform > Timer | Input > Controller
#include "game/intc_globals.h"

#include "game/types.h"

struct Port { u8 pad0[0x3c]; u8 *response; u8 pad40[4]; u8 index; u8 count; u8 pad46[0xa2]; u8 id; };
struct Device { volatile u8 data; u8 pad1[3]; volatile u16 status; u8 pad6[8]; volatile u16 control; };
extern struct Device *D_8009C78C;
extern volatile u32 *gInterruptStatusPtr __asm__("D_8009C788");
extern u32 D_8009C76C;
extern u32 D_800A6E18, D_800A9448;
extern void func_8008C890(u32);
extern s32 func_8008C8B0(void);

/**
 * @brief Exchanges one serial byte with a controller or memory card over PS1 SIO0 with Root Counter 2 timeouts.
 *
 * Selects SIO0 control 0x88 (or 0x22 for multitap 0x8x packets), waits for JOY_STAT ready (bit 1), arms timer 400 via func_8008C890, reads JOY_DATA, updates JOY_CTRL, polls I_STAT bit 0x80 with an inline Root Counter 2 timeout (returning -2), applies inter-byte delay when port->id != 8 && D_8009C76C == 2, writes value to JOY_DATA, and appends to port->response. Category Platform > Controller Input | Platform > Timer | Input > Controller follows SIO0/I_STAT/Timer2 MMIO.
 *
 * @param port Serial port state struct (+0x3C response buffer, +0x44 index, +0x45 count, +0xE8 id).
 * @param value_ Outbound command/data byte written to SIO0 JOY_DATA.
 * @return Received byte (0..255), or -2 on I_STAT ACK timeout.
 *
 * @par Globals
 * D_8009C78C/D_8009C788 - SIO0 (0x1F801040) and I_STAT (0x1F801070) ptrs; D_8009C76C/D_800A6E18/D_800A9448 - mode and timer threshold/start.
 *
 * @par Control flow
 * Status spin, ACK wait loop with Timer 2 (0x1F801120/24/28) wrap/prescale timeout returning -2, optional delay wait, and response append.
 *
 * @par Calls
 * func_8008C890 - captures Timer 2 baseline and timeout; func_8008C8B0 - polls 60-tick inter-byte delay.
 *
 * @par Side effects
 * Reads/writes SIO0 MMIO and Timer 2, updates D_800A6E18/D_800A9448, increments port->count/index, and stores byte in port->response.
 *
 * @par Callers
 * Confirmed: func_8008B810 (0x8008B8D0), func_8008B8E8 (0x8008B94C), func_8008B9A8 (0x8008B9F8), func_8008BA34 (0x8008BB08) / Likely: none / Unknown: none.
 */
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
    while (!(*gInterruptStatusPtr & 0x80)) {
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
