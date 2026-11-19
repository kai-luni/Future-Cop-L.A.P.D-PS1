// Categories: Game Engine > Game State > Reset
#include "game/types.h"

extern void *gScratchBlockPtr __asm__("D_8009F68C");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void gRetireAllRibbons(void) __asm__("func_800734BC");
extern void gTeardownWorldObjects(s32 arg0) __asm__("func_8003F854");
extern void func_8004ABBC(void);
extern void func_80029EB8(void);
extern void func_80059DB0(s32 arg0);
extern void func_80069D58(void);
extern void func_80049410(void);
extern void func_8002A5AC(void);
extern void func_8005A290(void);
extern void gReleaseMapLightData(void) __asm__("func_8004963C");
extern void func_80083428(void);
extern void gFreeMapZoneList(void) __asm__("func_80031990");
extern void func_8001B448(void);
extern void func_80013AB4(void);
extern void func_800634F4(void);
extern void func_80061860(void);
extern void func_80047868(void);
extern void gFreeCameraList(void) __asm__("func_80017764");
extern void gRetireAndFreeAllEntities(void) __asm__("func_8004CCDC");
extern void func_8006A88C(void);
extern void func_80060E50(void);
extern void func_80060A00(void);
extern void func_8005A068(void);
extern void gReleaseAllResources(void) __asm__("func_80019BA4");
extern void func_80023BF8(void);
extern void gFreeReciprocalTable(void) __asm__("func_800198DC");
extern void func_80019D88(void);
extern void func_80069D04(void);
extern void func_80086164(void);
extern void func_80017D20(void);

/**
 * @brief Global subsystem reset: frees the mission block and re-initialises
 *        every pool and manager in a fixed order.
 *
 * @param arg0 Forwarded to func_80059DB0 (the entity-world reset).
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F68C != 0 is freed via func_800181D4. Then 30 init calls run in
 * order: func_800734BC, func_8003F854(2), func_8004ABBC, func_80029EB8,
 * func_80059DB0(arg0), func_80069D58, func_80049410, func_8002A5AC,
 * func_8005A290, func_8004963C, func_80083428, func_80031990,
 * func_8001B448, func_80013AB4, func_800634F4, func_80061860,
 * func_80047868, func_80017764, func_8004CCDC, func_8006A88C,
 * func_80060E50, func_80060A00, func_8005A068, func_80019BA4,
 * func_80023BF8, func_800198DC, func_80019D88, func_80069D04,
 * func_80086164, func_80017D20.
 *
 * @par Likely role
 * The between-missions reset of the frame driver func_80025B7C: the
 * order mirrors the boot sequence (effects, text, sound, map, camera,
 * HUD), so a new mission starts from a clean slate without a full reboot.
 *
 * @par Unknown
 * Which subsystem each callee owns.
 *
 * @par Callers
 * func_80025B7C.
 */
void gResetMissionSubsystems(s32 arg0) __asm__("func_80024D14");

void gResetMissionSubsystems(s32 arg0)
{
    if (gScratchBlockPtr != 0) {
        gFreeBlock(gScratchBlockPtr);
    }
    gRetireAllRibbons();
    gTeardownWorldObjects(2);
    func_8004ABBC();
    func_80029EB8();
    func_80059DB0(arg0);
    func_80069D58();
    func_80049410();
    func_8002A5AC();
    func_8005A290();
    gReleaseMapLightData();
    func_80083428();
    gFreeMapZoneList();
    func_8001B448();
    func_80013AB4();
    func_800634F4();
    func_80061860();
    func_80047868();
    gFreeCameraList();
    gRetireAndFreeAllEntities();
    func_8006A88C();
    func_80060E50();
    func_80060A00();
    func_8005A068();
    gReleaseAllResources();
    func_80023BF8();
    gFreeReciprocalTable();
    func_80019D88();
    func_80069D04();
    func_80086164();
    func_80017D20();
}
