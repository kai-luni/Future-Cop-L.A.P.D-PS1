// Categories: Game Engine > Object Coordination
#include "game/types.h"
typedef struct Obj { s32 value; u8 pad[24]; s32 id; } Obj;
typedef struct Slot { s8 id; u8 type; u8 active; u8 pad; Obj *a; Obj *b; s32 time; } Slot;
extern Obj *D_8009F270, *D_8009F274;
extern Slot gObjectSlots[] __asm__("D_800A0A00");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 func_80023528(s32, s32 *);
extern void gFreeBlock(Obj *) __asm__("func_800181D4");
extern s32 func_80021CB0(s32, s32, s32, void *, s32);
extern void func_80023500(void);

/**
 *  @brief Processes a pending pair of objects and optionally records an operation slot.
 *
 *  Matching object identifiers and a successful eligibility check permit an operation in a free slot.
 *  A successful operation records both objects and a deadline 1500 units after the global clock.
 *  @par Globals
 *  D_8009F270 and D_8009F274 hold the pending pair; D_800A0A00 holds eight slots; D_8009FA2C supplies the clock.
 *  @par Control flow
 *  If both objects exist, the function checks their identifiers and scans for a slot with id -1.
 *  Validation or capacity failure releases both objects; a negative operation result leaves the slot unchanged.
 *  @par Calls
 *  func_80023528 checks eligibility; func_800181D4 releases objects on validation failure.
 *  func_80021CB0 starts the operation with func_80023500 as its callback.
 *  @par Side effects
 *  May release objects or populate a slot; clears both pending pointers whenever both were present.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800236FC(void)
{
    u16 i = 0;
    u8 failed = 0;
    s32 result;
    s32 time;
    if (D_8009F270 && D_8009F274) {
        if (D_8009F270->id == D_8009F274->id && func_80023528(D_8009F270->id, 0)) {
            while (i < 8) {
                if (gObjectSlots[i].id == -1) break;
                i++;
            }
            if (i >= 8) failed = 1;
        } else failed = 1;
        if (failed) {
            gFreeBlock(D_8009F270);
            gFreeBlock(D_8009F274);
        } else {
            result = func_80021CB0(D_8009F270->value, D_8009F274->value, -1, func_80023500, i);
            if (result >= 0) {
                gObjectSlots[i].active = 1;
                time = gFrameTickCounter + 1500;
                gObjectSlots[i].id = result;
                gObjectSlots[i].time = time;
                gObjectSlots[i].type = D_8009F270->id;
                gObjectSlots[i].a = D_8009F270;
                gObjectSlots[i].b = D_8009F274;
            }
        }
        D_8009F274 = 0;
        D_8009F270 = 0;
    }
}
