// Categories: Input > DualShock | Platform > Controller
typedef unsigned char u8;
typedef unsigned int u32;
typedef struct {
    u8 port, active, values[2], slots[2], pad[2];
    int times[2];
    u8 command[6], flag, pad17;
} State;
extern State gPadActuators[] __asm__("D_800A6FF8");
extern volatile int D_8009FA2C;
extern int func_80089E8C(int);
extern int func_8008A044(int, int, int);
extern void func_8008A240(int, u8 *, int);
extern int func_8008A1C0(int, u8 *);
extern int func_80089F4C(int, int, int);

/**
 * @brief Manages DualShock actuator alignment, rumble motor expiration, and motor commands for a pad port.
 *
 * Expires active rumble timers in D_800A6FF8[arg].times[0..1] against D_8009FA2C, polls pad mode via func_80089E8C, resets actuator state on mode 1, sends motor values via func_8008A240 and configures actuator alignment via func_8008A044/func_8008A1C0 when mode == 6 (or enables immediately on mode 2), or updates motor values from times[0] when active and !func_80089F4C(port, 2, 0). Category Input > DualShock | Platform > Controller follows the actuator slots and DualShock pad library calls.
 *
 * @param arg Logical controller index (0 or 1; values >= 2 return immediately).
 *
 * @par Globals
 * D_800A6FF8 - 2-slot actuator state array (port, active, values, slots, times, command, flag); D_8009FA2C - frame tick counter.
 *
 * @par Control flow
 * Port bounds check, 2-motor timeout loop, mode == 1 reset, active/inactive branch (mode 2/6 probe vs func_80089F4C check).
 *
 * @par Calls
 * func_80089E8C - query pad state; func_8008A240 - send actuator bytes; func_8008A044 - probe actuators; func_8008A1C0 - align actuators; func_80089F4C - query mode.
 *
 * @par Side effects
 * Updates D_800A6FF8[arg] motor values, slots, timers, command buffer, and active/flag fields, and dispatches controller actuator commands.
 *
 * @par Callers
 * Confirmed: func_8001EF8C (0x8001EFB4 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_8001EC94(u32 arg)
{
    State *s;
    int i, mode, n, count, kind;
    u8 port;
    volatile u8 *p;
    register int fill __asm__("$3");
    register int one __asm__("$2");
    register u8 *entry __asm__("$2");
    int slot;
    if (arg >= 2) return;
    s = &gPadActuators[arg];
    port = s->port;
    for (i = 0; i < 2; i++) {
        if (s->times[i] && s->times[i] < D_8009FA2C) {
            entry = (u8 *)s + i;
            __asm__ volatile("" : "=r"(entry) : "0"(entry));
            slot = entry[4];
            if (slot != 255) s->values[slot] = 0;
            s->times[i] = 0;
        }
    }
    mode = func_80089E8C(port);
    if (!mode) return;
    one = 1;
    __asm__ volatile("" : "=r"(one) : "0"(one));
    if (mode == one) {
        __asm__ volatile("" : "=r"(s) : "0"(s));
        i = one;
        __asm__ volatile("" : : "r"(i));
        s->active = 0;
        s->flag = 0;
        p = (u8 *)s + 1;
        do { p[2] = 0; p--; i--; } while (i >= 0);
        for (i = 1; i >= 0; i--) s->times[i] = 0;
    }
    if (!s->active) {
        func_8008A240(port, s->values, 2);
        if (mode == 2) goto enable;
        if (mode != 6) return;
        n = 0;
        fill = 255;
        __asm__ volatile("" : : "r"(fill));
        i = 5;
        __asm__ volatile("" : : "r"(i));
        p = (u8 *)s + 5;
        for (; i >= 0; i--) { p[16] = fill; p--; }
        fill = 255;
        __asm__ volatile("" : : "r"(fill));
        i = 1;
        __asm__ volatile("" : : "r"(i));
        p = (u8 *)s + 1;
        for (; i >= 0; i--) { p[4] = fill; p--; }
        count = func_8008A044(port, -1, 0);
        if (count >= 3) count = 2;
        for (i = 0; i < count; i++) {
            kind = func_8008A044(port, i, 3);
            if (kind == 0) {
                if (s->slots[0] == 255) {
                    s->command[i] = n;
                    s->slots[0] = n++;
                    s->flag = 1;
                }
            } else if (kind == 1) {
                if (s->slots[1] == 255) {
                    s->command[i] = n;
                    s->slots[1] = n++;
                    s->flag = kind;
                }
            }
        }
        if (func_8008A1C0(port, s->command)) {
        enable:
            s->active = 1;
        }
    } else if (!func_80089F4C(port, 2, 0)) {
        if (s->times[0]) {
            s->values[0] = 64;
            s->values[1] = 1;
        } else {
            s->values[0] = 64;
            s->values[1] = 0;
        }
    }
}
