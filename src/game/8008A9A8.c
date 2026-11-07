typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct { u8 pad[0x3c]; u8 *buffer; u8 *other; u8 index; u8 count; } State;
typedef struct { volatile u8 data; u8 pad[3]; volatile u16 status; u8 pad2[8]; volatile u16 control; } Port;
extern Port *D_8009C78C;
extern volatile u32 *D_8009C788;
extern volatile u32 D_800A6E18, D_800A9448;
extern int func_8008C8B0(void);
int func_8008A9A8(State *arg, int argvalue)
{
    register State *state __asm__("$16") = arg;
    register int value __asm__("$18");
    int result;
    register int control __asm__("$6");
    register unsigned int timer __asm__("$4");
    unsigned int status;
    register Port *port __asm__("$5");
    __asm__("" : "+r"(state));
    value = argvalue;
    if (value < 0) {
        register Port *negport __asm__("$3");
        result = D_8009C78C->data;
        state->index = 0xff;
        state->count = 1;
        *state->other = ~value;
        negport = D_8009C78C;
        if (!(negport->status & 1)) {
            do { __asm__ volatile(""); } while (!(negport->status & 1));
        }
        while (!func_8008C8B0()) {}
        {
            register int inverted __asm__("$2") = ~value;
            __asm__("" : "+r"(inverted));
            negport = D_8009C78C;
            negport->data = inverted;
        }
        return result;
    }
    control = 0x88;
    if ((((int)*(signed char *)state->buffer & 255) >> 4) == 8 && state->index >= 9)
        control = 0x22;
    timer = *(volatile u16 *)0x1f801120;
    port = D_8009C78C;
    status = port->status;
    D_800A9448 = 0x1ae;
    D_800A6E18 = timer;
    if (!(status & 2)) {
        register Port *poll __asm__("$3") = port;
        do { __asm__ volatile(""); } while (!(poll->status & 2));
    }
    {
        Port *p = D_8009C78C;
        result = p->data;
        p->control = control;
    }
    while (!(*D_8009C788 & 0x80)) {
        if (func_8008C8B0()) return -20;
    }
    D_8009C78C->data = value;
    state->count++;
    state->buffer[state->index] = result;
    {
        register unsigned int idx __asm__("$2") = state->index;
        __asm__("" : "+r"(idx));
        idx++;
        __asm__("" : "+r"(idx));
        state->index = idx;
    }
    return result;
}
