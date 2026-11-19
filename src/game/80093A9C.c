// Categories: Graphics > GPU Command Lists > Draw Mode | Platform > GPU
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u8 pad[3];
    u8 length;
    u32 command;
    u32 data;
} Packet;

extern u32 gBuildDrawModeCommand(int, int, u32) __asm__("func_80093F58");
extern u32 gBuildTexWindowCommand(int) __asm__("func_800940C4");
void gSetDrawTPage(Packet *packet, int arg1, int arg2, u16 arg3, int arg4) __asm__("func_80093A9C");


/**
 * @brief Builds a two-word draw-mode packet: GP0 0xE1 (texpage/dither) plus the
 *        GP0 0xE2 texture-window word.
 *
 * @param packet Target packet (length = 2).
 * @param arg1, arg2, arg3 Forwarded to func_80093F58, which assembles the
 *        command word: 0xE1000000 | dither | draw-to-display | (tpage & 0x9FF).
 * @param arg4 Source of the texture window; func_800940C4 converts it (eight
 *        byte values divided by 8) into 0xE2000000 | (y<<15) | (x<<10) |
 *        (h<<5) | w. A NULL pointer produces a zero word.
 * @return Nothing.
 *
 * @par Callers
 * None in the symbol table; the sprite and model paths emit it inline.
 *
 * @par Environment path
 * Builds a two-word GPU draw-mode packet containing a GP0 0xE1 command from
 * func_80093F58 and a GP0 0xE2 texture-window command from func_800940C4.
 */
void gSetDrawTPage(Packet *packet, int arg1, int arg2, u16 arg3, int arg4)
{
    packet->length = 2;
    packet->command = gBuildDrawModeCommand(arg1, arg2, arg3);
    packet->data = gBuildTexWindowCommand(arg4);
}
