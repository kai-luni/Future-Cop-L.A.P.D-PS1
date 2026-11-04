#include "game/types.h"

struct Port;

struct Port {
    u8 pad_00[0x14];
    s32 field14;
    s32 (*field18)(struct Port *);
    u8 pad_1C[0x14];
    u8 *field30;
    u8 pad_34[2];
    u8 field36;
    u8 field37;
    u8 pad_38[4];
    u8 *response;
    u8 pad_40[4];
    u8 field44;
    u8 pad_45;
    u8 sequence_step;
    u8 info_record_index;
    u8 combination_bytes_remaining;
    u8 communication_state;
    u8 field4A;
    u8 pad_4B[5];
    u8 field50;
    u8 pad_51[0x97];
    u8 current_id;
};

extern void (*D_8009C72C)(struct Port *port);
extern s32 func_8008C858(struct Port *port);
extern s32 func_8008AF10(struct Port *port);

void func_8008C518(struct Port *port)
{
    u32 saved_id;
    u8 new_id;
    s32 i;
    s32 step;
    register s32 delta __asm__("$2");

    saved_id = port->current_id;
    new_id = port->response[0] >> 4;
    port->current_id = new_id;
    if (new_id == 0xF) {
        port->current_id = saved_id;
    } else {
        port->field30[0] = 0;
        port->field30[1] = port->response[0];
        for (i = 2; i < port->field44; i++) {
            port->field30[i] = port->response[i];
        }
    }

    if ((port->response[1] == 0 &&
         (port->sequence_step != 1 || port->field14 != 0) &&
         port->field50 == 0) ||
        (func_8008C858(port) == 0 &&
         port->field37 == 0 &&
         port->field4A == 0 &&
         port->current_id != saved_id)) {
        D_8009C72C(port);
    }

    port->field4A = 0;
    if ((u8)(port->sequence_step - 2) < 0xFC &&
        port->response[0] != 0xF3) {
        D_8009C72C(port);
    }

    step = port->sequence_step;
    if (step != 0 && step != 0xFF) {
        if (port->field36 == 0) {
            return;
        }
    }

    switch (step) {
    case 0:
        if (port->current_id != 0) {
            port->communication_state = 1;
            port->sequence_step++;
        }
        return;

    case 1:
        port->info_record_index = 0;
        port->sequence_step++;
        return;

    case 0xFE:
        port->sequence_step = step + 1;
        return;

    case 0xFF:
        return;

    default:
        if (port->field18 != 0) {
            delta = port->field18(port);
        } else {
            delta = func_8008AF10(port);
        }
        port->sequence_step += delta;
        return;
    }
}
