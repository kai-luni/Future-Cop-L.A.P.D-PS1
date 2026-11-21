// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/cdrom_globals.h"

typedef unsigned char u8;
typedef unsigned int u32;

extern char gCdInitLogMsg[] __asm__("D_8001257C");
extern char gCdInitAddrFmt[] __asm__("D_80012588");
extern char gCdStateBlock[] __asm__("D_8009CB88");
extern void gCdIrqHandlerAddress(void) __asm__("D_8008FE28");
extern u8 gLastResultByte __asm__("D_8009C8C5");
extern u8 gLastCommandResult __asm__("D_8009C8C4");
extern u32 gCdReadyCallback __asm__("D_8009C8AC");
extern u32 gCdSyncCallback __asm__("D_8009C8A8");
extern u32 gCdIrqExtraByte __asm__("D_8009C8B8");
extern u32 gCdStatusByte __asm__("D_8009C8B4");
extern volatile u8 *gCdromParameterPtr __asm__("D_8009CB74");
extern volatile u8 *gCdromRequestStatusPtr __asm__("D_8009CB78");
extern volatile u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u32 *gComAccessDelayPtr __asm__("D_8009CB7C");
extern volatile u8 gCdStateBytes[] __asm__("D_8009CB84");
extern void func_8008FF08(char *);
extern void func_8008E000(char *, char *);
extern void gResetIrqDispatch(void) __asm__("func_80091480");
extern void gSetIrqCallback(int, void (*)(void)) __asm__("func_800914B0");
extern int gIssueCdCommand(int, int, int, int) __asm__("func_8008F244");
extern int gCdSyncLoop(int, int) __asm__("func_8008ECFC");
int gInitCdSubsystem(void) __asm__("func_8008F8E8");


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
 *
 * @par Role and path
 * Initializes the PsyQ CD-ROM subsystem by logging the CD_init banner and
 * state block D_8009CB88, clearing driver callbacks and status globals, and
 * installing func_8008FE28 on IRQ slot 2 via func_80091480 and func_800914B0.
 * It then clears pending CD-ROM interrupts, sets D_8009CB7C (0x1F801020) to
 * 0x1325, and issues CdlNop, CdlReset, CdlDemute via func_8008F244 followed
 * by func_8008ECFC sync. Driven by func_8008DF30 and func_8008E04C.
 */
int gInitCdSubsystem(void)
{
    register int z __asm__("$0");
    register volatile u8 *state __asm__("$3");
    register char *first __asm__("$4");
    first = gCdInitLogMsg;
    __asm__("" : "+r"(first));
    func_8008FF08(first);
    func_8008E000(gCdInitAddrFmt, gCdStateBlock);
    gLastResultByte = 0;
    __asm__ volatile("" : : : "memory");
    gLastCommandResult = 0;
    __asm__ volatile("" : : : "memory");
    gCdReadyCallback = 0;
    __asm__ volatile("" : : : "memory");
    gCdSyncCallback = 0;
    __asm__ volatile("" : : : "memory");
    gCdIrqExtraByte = 0;
    __asm__ volatile("" : : : "memory");
    {
        register u32 *p __asm__("$1") = (u32 *)0x800A0000;
        __asm__("" : "+r"(p));
        *(u32 *)((char *)p - 0x374c) = 0;
        gResetIrqDispatch();
    }
    {
        register void (*cb)(void) __asm__("$5") = gCdIrqHandlerAddress;
        __asm__("" : "+r"(cb));
        gSetIrqCallback(2, cb);
    }
    *gCdromIndexStatusPtr = 1;
    while (*gCdromRequestStatusPtr & 7) {
        *gCdromIndexStatusPtr = 1;
        *gCdromRequestStatusPtr = 7;
        *gCdromParameterPtr = 7;
    }
    {
        register int a0 __asm__("$4") = 1;
        register int a1 __asm__("$5") = 0;
        register volatile u8 *port __asm__("$7");
        __asm__("" : "+r"(a0), "+r"(a1));
        state = gCdStateBytes;
        state[2] = 0;
        state[1] = state[2];
        port = gCdromIndexStatusPtr;
        __asm__("" : "+r"(state), "+r"(port));
        state[0] = 2;
        *port = 0;
        *gCdromRequestStatusPtr = 0;
        *gComAccessDelayPtr = 0x1325;
        gIssueCdCommand(a0, a1, z, z);
    }
    if (gCdStatusByte & 0x10)
        gIssueCdCommand(1, 0, z, z);
    if (gIssueCdCommand(10, 0, z, z))
        return -1;
    if (gIssueCdCommand(12, 0, z, z))
        return -1;
    if (gCdSyncLoop(0, z) != 2)
        return -1;
    return 0;
}
