// Categories: Platform > Hardware Registers
#include "game/cdrom_globals.h"

#include "game/types.h"

struct State {
    u8 pad[0x180];
    u16 a, b;
    u8 pad2[0x26];
    u16 c;
    u8 pad3[4];
    u16 d, e;
    u8 pad4[4];
    u16 f, g;
};
extern struct State *D_8009CB80;
extern u8 *D_8009CB70, *D_8009CB74, *D_8009CB78;
extern u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");

/**
 *  @brief Initializes shared state fields and writes a short register sequence.
 *
 *  Two fields are initialized conditionally, while three others are always
 *  set before the indexed register writes.
 *  @return Always zero.
 *  @par Globals
 *  D_8009CB80 points to the state whose fields are initialized.
 *  D_8009CB6C selects register banks; D_8009CB70, D_8009CB74 and
 *  D_8009CB78 point to registers written in the selected banks.
 *  @par Control flow
 *  Fields a and b are set only when both f and g are zero.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates shared state and writes the register sequence for banks 2 and 3.
 *  @par Callers
 *  Confirmed: func_8008DF30, func_8008E04C.
 */
s32 func_8008F7AC(void)
{
    volatile u8 data[4];
    if (D_8009CB80->f == 0 && D_8009CB80->g == 0) {
        D_8009CB80->a = 0x3fff;
        D_8009CB80->b = 0x3fff;
    }
    D_8009CB80->d = 0x3fff;
    D_8009CB80->e = 0x3fff;
    D_8009CB80->c = 0xc001;
    data[2] = 0x80;
    data[0] = 0x80;
    data[3] = 0;
    data[1] = 0;
    *gCdromIndexStatusPtr = 2;
    *D_8009CB74 = data[0];
    *D_8009CB78 = data[1];
    *gCdromIndexStatusPtr = 3;
    *D_8009CB70 = data[2];
    *D_8009CB74 = data[3];
    *D_8009CB78 = 0x20;
    return 0;
}
