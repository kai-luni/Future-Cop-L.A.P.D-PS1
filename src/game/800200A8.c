// Categories: Platform > SPU > Reset | Platform > Interrupts
#include "game/types.h"

extern void func_8008C9C0(void);
extern void func_8008D6E0(s32 mode);
extern void func_8008D660(s32 channel);
extern void func_8008D74C(void);
extern void func_8008C950(void);
extern void func_8008C9E0(void);

/**
 * @brief Interrupt-safe SPU control reset sequence.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Under func_8008C9C0 (interrupts masked): func_8008D6E0(0),
 * func_8008D660(0), func_8008D74C(), func_8008C950(); then func_8008C9E0
 * unmask.
 *
 * @par Likely role
 * The SPU stop/reset shim called by the sound teardown func_8001E4E4:
 * the zero arguments likely disable transfer and IRQ sources before the
 * common reset, the masking prevents a half-reset state from firing.
 *
 * @par Unknown
 * The exact register effects of the four SPU entries.
 *
 * @par Callers
 * func_8001E4E4.
 */
void func_800200A8(void)
{
    func_8008C9C0();
    func_8008D6E0(0);
    func_8008D660(0);
    func_8008D74C();
    func_8008C950();
    func_8008C9E0();
}
