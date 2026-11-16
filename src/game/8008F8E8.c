// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/cdrom_globals.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern char D_8001257C[];
extern char D_80012588[];
extern char D_8009CB88[];
extern void D_8008FE28(void);
extern u8 D_8009C8C5, D_8009C8C4;
extern u32 D_8009C8AC, D_8009C8A8, D_8009C8B8, D_8009C8B4;
extern volatile u8 *D_8009CB74;
extern volatile u8 *gCdromRequestStatusPtr __asm__("D_8009CB78");
extern volatile u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u32 *D_8009CB7C;
extern volatile u8 D_8009CB84[];
extern void func_8008FF08(char *);
extern void func_8008E000(char *, char *);
extern void func_80091480(void);
extern void func_800914B0(int, void (*)(void));
extern int func_8008F244(int, int, int, int);
extern int func_8008ECFC(int, int);

/**
 * @brief Initializes the PsyQ CD-ROM subsystem, interrupt callback, and drive state.
 *
 * Logs "CD_init:" and the D_8009CB88 state block address, clears CD callbacks and flags, invokes D_8009DD10 via func_80091480 and registers func_8008FE28 on IRQ slot 2 via func_800914B0, clears pending CD interrupts, sets D_8009CB7C (0x1F801020) to 0x1325, and issues CdlNop (1), CdlReset (10), CdlDemute (12), and CdlSync.
 *
 * @param void No parameters.
 * @return 0 if initialization and sync (status == 2) succeed, or -1 if CdlReset, CdlDemute, or func_8008ECFC fails.
 *
 * @par Globals
 * D_8001257C/D_80012588 - format strings; D_8009CB88/D_8009CB84 - CD state table; D_8009C8A8..D_8009C8C5 - callbacks/flags; D_8009CB6C..D_8009CB7C - CD-ROM/delay register pointers.
 *
 * @par Control flow
 * Straight-line setup with a poll loop clearing (*D_8009CB78 & 7), an optional retry of command 1 if D_8009C8B4 & 0x10, and three error-checking exits.
 *
 * @par Calls
 * func_8008FF08 - prints banner; func_8008E000 - prints addr; func_80091480/func_800914B0 - dispatches D_8009DD10 and installs IRQ callback func_8008FE28; func_8008F244 - sends CD commands; func_8008ECFC - waits for ready status.
 *
 * @par Side effects
 * Clears CD globals, writes CD-ROM hardware ports and D_8009CB7C (0x1F801020), resets D_8009CB84, and installs the CD IRQ handler.
 *
 * @par Callers
 * Confirmed: func_8008DF30 (0x8008DF38), func_8008E04C (0x8008E074) / Likely: none / Unknown: none.
 */
int func_8008F8E8(void)
{
    register int z __asm__("$0");
    register volatile u8 *state __asm__("$3");
    register char *first __asm__("$4");
    first = D_8001257C;
    __asm__("" : "+r"(first));
    func_8008FF08(first);
    func_8008E000(D_80012588, D_8009CB88);
    D_8009C8C5 = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8C4 = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8AC = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8A8 = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8B8 = 0;
    __asm__ volatile("" : : : "memory");
    {
        register u32 *p __asm__("$1") = (u32 *)0x800A0000;
        __asm__("" : "+r"(p));
        *(u32 *)((char *)p - 0x374c) = 0;
        func_80091480();
    }
    {
        register void (*cb)(void) __asm__("$5") = D_8008FE28;
        __asm__("" : "+r"(cb));
        func_800914B0(2, cb);
    }
    *gCdromIndexStatusPtr = 1;
    while (*gCdromRequestStatusPtr & 7) {
        *gCdromIndexStatusPtr = 1;
        *gCdromRequestStatusPtr = 7;
        *D_8009CB74 = 7;
    }
    {
        register int a0 __asm__("$4") = 1;
        register int a1 __asm__("$5") = 0;
        register volatile u8 *port __asm__("$7");
        __asm__("" : "+r"(a0), "+r"(a1));
        state = D_8009CB84;
        state[2] = 0;
        state[1] = state[2];
        port = gCdromIndexStatusPtr;
        __asm__("" : "+r"(state), "+r"(port));
        state[0] = 2;
        *port = 0;
        *gCdromRequestStatusPtr = 0;
        *D_8009CB7C = 0x1325;
        func_8008F244(a0, a1, z, z);
    }
    if (D_8009C8B4 & 0x10)
        func_8008F244(1, 0, z, z);
    if (func_8008F244(10, 0, z, z))
        return -1;
    if (func_8008F244(12, 0, z, z))
        return -1;
    if (func_8008ECFC(0, z) != 2)
        return -1;
    return 0;
}
