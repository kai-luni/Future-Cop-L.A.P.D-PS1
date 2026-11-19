// Categories: Platform > Controller Input
#include "game/types.h"

extern s32 D_8009F8B0[];
extern u8 gRumbleEnabled[] __asm__("D_8009F8D8");
extern s32 func_80046C84(s32, s32);
extern s32 gGetPadType(s32) __asm__("func_8001EF34");
extern u8 gIsPadRumbleReady(s32) __asm__("func_8001EAEC");
extern u8 gIsPadRumbleEnabled(s32) __asm__("func_8001EB2C");
extern void gSetPadRumbleEnabled(s32, s32) __asm__("func_8001EC04");
extern void gSetPortSmallRumble(s32, s32) __asm__("func_8001E96C");

/**
 * @brief Filters the pad state for one port and toggles its vibration pair.
 *
 * A button counts as new when it is set in arg0 and was clear in the mask
 * kept at D_8009F8B0. In that case a sound is started through
 * func_80046C84(0x48, 0), the port is mapped to 0/1 and, if a pad is present
 * (func_8001EF34) and reports connected (func_8001EAEC), the toggle at
 * D_8009F8D8[port] flips. On the rising edge the current motor mode is read
 * with func_8001EB2C, motor 1 is started, and the saved mode is written back,
 * so the pad shakes once per press.
 *
 * @param arg0 Button bits to test.
 * @param arg1 Pad port (0 or 1).
 * @return The button bits that were passed in; the return value is the input.
 *
 * @par Globals
 * D_8009F8B0 - mask of buttons already seen; D_8009F8D8 - per-port toggle.
 *
 * @par Control flow
 * Two-level guard (port equality, then new-button bit test) around the toggle.
 *
 * @par Calls
 * func_80046C84 - sound cue. func_8001EF34 - pad present. func_8001EAEC - pad
 * connected. func_8001EB2C - read motor mode. func_8001EC04 - set motor,
 * func_8001E96C - apply motor state.
 *
 * @par Side effects
 * Writes the toggle byte and the pad motor registers.
 *
 * @par Callers
 * Unknown: no direct edge; the signature suggests a per-frame pad hook.
 */
s32 gToggleVibrationPair(s32 arg0, s32 arg1) __asm__("func_8007C940");

s32 gToggleVibrationPair(s32 arg0, s32 arg1)
{
    register s32 input __asm__("$2");
    register s32 player __asm__("$17") = arg1;
    register s32 buttons __asm__("$18") = arg0;
    register s32 *base __asm__("$2");
    register s32 tailv __asm__("$2");
    s32 port;
    u8 saved;

    __asm__("" : "=r"(player) : "0"(player));
    __asm__("" : "=r"(buttons) : "0"(buttons));
    if (player == input) {
        base = D_8009F8B0;
        __asm__("" : "=r"(base) : "0"(base));
        if ((buttons & ~base[player]) < 0) {
            func_80046C84(0x48, 0);
            port = (player == 1);
            if (gGetPadType(port)) {
                register s32 connected __asm__("$2");
                connected = gIsPadRumbleReady(port) & 0xff;
                __asm__("" : "=r"(connected) : "0"(connected));
                if (!connected) return buttons;
                {
                register s32 toggle __asm__("$2");
                register u8 *p __asm__("$3");
                register s32 result __asm__("$2") = buttons;
                __asm__("" : : "r"(result));
                p = &gRumbleEnabled[player];
                __asm__("" : "=r"(p) : "0"(p));
                toggle = *p;
                __asm__("" : "=r"(toggle) : "0"(toggle));
                toggle ^= 1;
                __asm__("" : "=r"(toggle) : "0"(toggle));
                *p = toggle;
                if (toggle) {
                    saved = gIsPadRumbleEnabled(player);
                    gSetPadRumbleEnabled(player, 1);
                    gSetPortSmallRumble(player, 1);
                    gSetPadRumbleEnabled(player, saved);
                }
                }
            }
        }
    }
    tailv = buttons;
    __asm__("" : "=r"(tailv) : "0"(tailv));
    return tailv;
}
