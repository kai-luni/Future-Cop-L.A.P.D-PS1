// Categories: Platform > Hardware Registers
#include "game/spu_globals.h"

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
extern struct State *gSpuRegsAltPtr __asm__("D_8009CB80");
extern u8 *gCdromParameterPtr __asm__("D_8009CB74");
extern u8 *gCdromCommandPtr __asm__("D_8009CB70");
extern u8 *gCdromRequestStatusPtr __asm__("D_8009CB78");
extern u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
s32 gInitCdAudioMix(void) __asm__("func_8008F7AC");


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
 *
 * @par Role and path
 * Initializes CD audio volume and control registers through gSpuRegsAltPtr
 * (D_8009CB80) and writes {0x80, 0, 0x80, 0} into CD-ROM register banks 2 and
 * 3 followed by 0x20 to enable CD audio mixing. Returns 0 and is driven by
 * func_8008DF30 and func_8008E04C.
 */
s32 gInitCdAudioMix(void)
{
    volatile u8 data[4];
    if (gSpuRegsAltPtr->f == 0 && gSpuRegsAltPtr->g == 0) {
        gSpuRegsAltPtr->a = 0x3fff;
        gSpuRegsAltPtr->b = 0x3fff;
    }
    gSpuRegsAltPtr->d = 0x3fff;
    gSpuRegsAltPtr->e = 0x3fff;
    gSpuRegsAltPtr->c = 0xc001;
    data[2] = 0x80;
    data[0] = 0x80;
    data[3] = 0;
    data[1] = 0;
    *gCdromIndexStatusPtr = 2;
    *gCdromParameterPtr = data[0];
    *gCdromRequestStatusPtr = data[1];
    *gCdromIndexStatusPtr = 3;
    *gCdromCommandPtr = data[2];
    *gCdromParameterPtr = data[3];
    *gCdromRequestStatusPtr = 0x20;
    return 0;
}
