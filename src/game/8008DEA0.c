// Categories: Platform > CD-ROM | Platform > Device Init
#include "game/types.h"
extern u8 D_8008DF6C[]; extern u8 D_8008DF94[]; extern u8 D_8008DFBC[]; extern u8 gCdInitFailMsg[] __asm__("D_8001231C");
extern u8 *gCdSyncCallback __asm__("D_8009C8A8");
extern u8 *gCdReadyCallback __asm__("D_8009C8AC");
extern u8 *gCdReadCallback __asm__("D_8009C810");
extern s32 gCdUseDmaFlag __asm__("D_8009C814");
s32 gStartCdInit(void) __asm__("func_8008DEA0");

extern s32 gInitCdHardware(void) __asm__("func_8008DF30"); extern void func_8008E000(u8 *str);
/**
 * @brief Initialises the CD-ROM driver, retrying five times.
 *
 * @return 1 when the driver came up, 0 when all five attempts failed (in which
 *         case the failure message is printed).
 *
 * @par Confirmed flow
 * The loop starts at i = 4 and calls func_8008DF30(); only a return value of
 * exactly 1 counts as success. On success the three driver tables are installed
 * (D_8009C8A8 = D_8008DF6C, D_8009C8AC = D_8008DF94, D_8009C810 = D_8008DFBC,
 * D_8009C814 = 0) and 1 is returned. After the fifth failure the inline
 * assembly calls func_8008E000("CdInit: Init failed\n") (the string sits at
 * D_8001231C) and returns 0.
 *
 * @par Globals
 * D_8009C8A8/D_8009C8AC/D_8009C810 (driver function tables),
 * D_8009C814 (driver state), D_8001231C (error message).
 *
 * @par Likely origin
 * The Sony libcd "CdInit" pattern: five retries, then a printf of
 * "CdInit: Init failed". The three installed pointers are the console/callback
 * vectors of that library.
 *
 * @par Callers
 * func_80076EB0 (boot) - a 0 result makes func_80076EB0 report failure and the
 * main loop never starts. Also func_8001C808 and func_8001D130.
 *
 * @par Role and path
 * Retries func_8008DF30 up to five times; on success (return value 1)
 * installs D_8008DF6C, D_8008DF94, and D_8008DFBC into D_8009C8A8,
 * D_8009C8AC, and D_8009C810, clears gCdUseDmaFlag (D_8009C814), and
 * returns 1. On five failures prints gCdInitFailMsg (D_8001231C) via
 * func_8008E000 and returns 0. Driven by func_80076EB0, func_8001C808, and
 * func_8001D130.
 */
s32 gStartCdInit(void)
{
    s32 i = 4;
    s32 one;
    s32 r;
again:
    r = gInitCdHardware();
    one = 1;
    __asm__ volatile("" : "+r"(one));
    if (r != one) {
        __asm__ volatile("");
        if (--i != -1)
            goto again;
        __asm__ volatile(
            ".set\tnoreorder\n\t"
            "lui $4,%%hi(D_8001231C)\n\t"
            "jal func_8008E000\n\t"
            "addiu $4,$4,%%lo(D_8001231C)\n\t"
            ".set\treorder"
            : : : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "$31", "memory");
        return 0;
    }
    gCdSyncCallback = D_8008DF6C;
    gCdReadyCallback = D_8008DF94;
    gCdReadCallback = D_8008DFBC;
    gCdUseDmaFlag = 0;
    return 1;
}
