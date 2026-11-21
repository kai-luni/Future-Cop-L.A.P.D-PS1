// Categories: Game Engine > Game State > Reset
#include "game/types.h"

extern void gReleaseActiveFrontEndResources(void) __asm__("func_8007852C");
extern void func_80029EB8(void);
extern void func_80069D58(void);
extern void func_8001B448(void);
extern void func_80047868(void);
extern void gReleaseAllResources(void) __asm__("func_80019BA4");
extern void func_80023BF8(void);
extern void gFreeReciprocalTable(void) __asm__("func_800198DC");
extern void func_80069D04(void);
extern void func_80017D20(void);

/**
 * @brief Subsystem teardown sequence: ten shutdown calls in a fixed order.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Runs func_8007852C (the object-registry free), func_80029EB8,
 * func_80069D58, func_8001B448, func_80047868, func_80019BA4,
 * func_80023BF8, func_800198DC, func_80069D04, func_80017D20.
 *
 * @par Likely role
 * The teardown counterpart of the global reset func_80024D14, called by
 * the frame driver func_80025B7C before a mission change: the order
 * mirrors the init sequence in reverse groups (registry first, display
 * last).
 *
 * @par Unknown
 * Which subsystem each callee owns.
 *
 * @par Callers
 * func_80025B7C.
 */
void gUnloadFrontEndSubsystems(void) __asm__("func_800249BC");

void gUnloadFrontEndSubsystems(void)
{
    gReleaseActiveFrontEndResources();
    func_80029EB8();
    func_80069D58();
    func_8001B448();
    func_80047868();
    gReleaseAllResources();
    func_80023BF8();
    gFreeReciprocalTable();
    func_80069D04();
    func_80017D20();
}
