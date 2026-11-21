// Categories: Platform > Memory Card | Platform > BIOS | Platform > Interrupts
#include "game/types.h"

extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gInitCardAndPatches(s32 mode) __asm__("func_8008D6E0");
extern void func_8008D660(s32 channel);
extern void gStartCardAndClearPad(void) __asm__("func_8008D74C");
extern void gInitMemoryCardBiosUnit(void) __asm__("func_8008C950");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");

/**
 * @brief Initialises the memory-card BIOS support inside a critical section.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Under func_8008C9C0 (interrupts masked): func_8008D6E0(0) (InitCARD with
 * the kernel card/pad table patches), func_8008D660(0) (BIOS vector A0:0xAD),
 * func_8008D74C() (StartCARD and ChangeClearPAD(0)), func_8008C950()
 * (BIOS vector A0:0x70, _bu_init); then func_8008C9E0 unmasks.
 *
 * @par Likely role
 * The card bootstrap of the pad/device layer initialisation func_8001E4E4:
 * the card service is started and the backup unit initialised before any
 * controller or save-file access. The zero arguments are the card mode (0)
 * and the unused A0:0xAD argument, and the masking keeps the BIOS table
 * rewrites from running with an interrupt half-way through.
 *
 * @par Unknown
 * Which of the patched kernel table entries belong to CARD and which to PAD;
 * the callees only name the BIOS vector numbers.
 *
 * @par Callers
 * func_8001E4E4.
 */
void gInitMemoryCardBios(void) __asm__("func_800200A8");

void gInitMemoryCardBios(void)
{
    gDisableCpuInterrupts();
    gInitCardAndPatches(0);
    func_8008D660(0);
    gStartCardAndClearPad();
    gInitMemoryCardBiosUnit();
    gEnableCpuInterrupts();
}
