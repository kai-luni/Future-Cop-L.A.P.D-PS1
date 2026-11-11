#include "game/types.h"

extern s32 D_8009F8B0[];
extern u8 D_8009F8D8[];
extern s32 func_80046C84(s32, s32);
extern s32 func_8001EF34(s32);
extern u8 func_8001EAEC(s32);
extern u8 func_8001EB2C(s32);
extern void func_8001EC04(s32, s32);
extern void func_8001E96C(s32, s32);

s32 func_8007C940(s32 arg0, s32 arg1)
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
            if (func_8001EF34(port)) {
                register s32 connected __asm__("$2");
                connected = func_8001EAEC(port) & 0xff;
                __asm__("" : "=r"(connected) : "0"(connected));
                if (!connected) return buttons;
                {
                register s32 toggle __asm__("$2");
                register u8 *p __asm__("$3");
                register s32 result __asm__("$2") = buttons;
                __asm__("" : : "r"(result));
                p = &D_8009F8D8[player];
                __asm__("" : "=r"(p) : "0"(p));
                toggle = *p;
                __asm__("" : "=r"(toggle) : "0"(toggle));
                toggle ^= 1;
                __asm__("" : "=r"(toggle) : "0"(toggle));
                *p = toggle;
                if (toggle) {
                    saved = func_8001EB2C(player);
                    func_8001EC04(player, 1);
                    func_8001E96C(player, 1);
                    func_8001EC04(player, saved);
                }
                }
            }
        }
    }
    tailv = buttons;
    __asm__("" : "=r"(tailv) : "0"(tailv));
    return tailv;
}
