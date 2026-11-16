// Categories: Platform > CD-ROM > Control
#include "game/cdrom_globals.h"

typedef unsigned char u8;
typedef unsigned int u32;
extern volatile u8 *gCdromParameterPtr __asm__("D_8009CB74");
extern volatile u8 *gCdromRequestStatusPtr __asm__("D_8009CB78");
extern volatile u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u32 *D_8009CB7C;
extern volatile u8 D_8009CB84[];

/**
 * @brief Clears pending CD-ROM hardware interrupts and resets CD status and bus delay.
 *
 * Selects CD register bank 1 via D_8009CB6C (0x1F801800) and loops acknowledging and
 * masking bits 0..2 in D_8009CB78/D_8009CB74 (0x1F801803/02) until clear, then
 * resets software state D_8009CB84[0..2], bank 0, and bus timing D_8009CB7C (0x1F801020).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009CB6C (0x1F801800), D_8009CB74 (0x1F801802), D_8009CB78 (0x1F801803), D_8009CB7C (0x1F801020) - CD/bus MMIO pointers; D_8009CB84 - 3-byte CD state.
 *
 * @par Control flow
 * Sets bank 1, spins while (*D_8009CB78 & 7) != 0 writing 1/7/7, then writes state[2]=state[1]=0, state[0]=2, bank 0, *D_8009CB78=0, and *D_8009CB7C=0x1325.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes PS1 CD-ROM MMIO registers 0x1F801800..03, COM_DELAY 0x1F801020 = 0x1325, and D_8009CB84[0..2].
 *
 * @par Callers
 * Confirmed: func_8008E0B8 (0x8008E0C0), func_8008ECFC (0x8008EE20), func_8008EF7C (0x8008F0A0), func_8008F244 (0x8008F4FC), func_8008FAC8 (0x8008FBCC).
 */
void func_8008F6D8(void)
{
    *gCdromIndexStatusPtr = 1;
    while (*gCdromRequestStatusPtr & 7) {
        *gCdromIndexStatusPtr = 1;
        *gCdromRequestStatusPtr = 7;
        *gCdromParameterPtr = 7;
    }
    {
        register volatile u8 *state __asm__("$3") = D_8009CB84;
        register volatile u8 *port __asm__("$4");
        state[2] = 0;
        state[1] = state[2];
        port = gCdromIndexStatusPtr;
        __asm__("" : "+r"(state), "+r"(port));
        state[0] = 2;
        *port = 0;
    }
    *gCdromRequestStatusPtr = 0;
    *D_8009CB7C = 0x1325;
}
