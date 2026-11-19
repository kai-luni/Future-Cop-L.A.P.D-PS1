// Categories: Input > Button Mapping
#include "game/types.h"

struct InputMaskMap {
    u32 masks[14];
};

extern struct InputMaskMap gPadMappingTable[2] __asm__("D_800AAC50");
extern u32 gPollPadButtons(void) __asm__("func_8001EF8C");
u32 gMapPadButtons(u32 profile) __asm__("func_8001E620");


/**
 *  @brief Maps the current pressed-input mask to a profile-specific result mask.
 *
 *  Fourteen configured masks are tested independently against the current input;
 *  each matching entry sets its assigned bit in the returned mask.
 *  @param profile Mapping profile index, valid for values zero and one.
 *  @return Mapped input bits, or zero for an invalid profile or no matches.
 *  @par Globals
 *  D_800AAC50 contains the two profiles and their fourteen input masks.
 *  @par Control flow
 *  Reads input first, then tests each mapping only when profile is less than two.
 *  @par Calls
 *  func_8001EF8C supplies the current pressed-input mask.
 *  @par Side effects
 *  No direct writes; the input query may have its own effects.
 *  @par Callers
 *  Confirmed: func_80057864.
 *
 * @par Role and path
 * Polls the current pressed-button mask via func_8001EF8C and remaps up to 14
 * buttons through gPadMappingTable[profile] into upper result bits when
 * profile is 0 or 1. Reads D_800AAC50 (gPadMappingTable) and returns 0 for
 * out-of-range profiles; called by func_80057864.
 */
u32 gMapPadButtons(u32 profile)
{
    register struct InputMaskMap *map __asm__("$5");
    register u32 pressed __asm__("$6");
    register u32 result __asm__("$4");
    register u32 offset __asm__("$3");
    register u32 returnValue __asm__("$2");
    register u32 firstMask __asm__("$2");
    register u32 secondMask __asm__("$3");
    register u32 firstSet __asm__("$2");

    pressed = gPollPadButtons();
    result = 0;
    if (profile < 2) {
        offset = (profile * 8) - profile;
        offset <<= 3;
        map = (struct InputMaskMap *)((u8 *)gPadMappingTable + offset);
        firstMask = map->masks[0];
        secondMask = map->masks[1];
        firstSet = (pressed & firstMask) != 0;
        result = firstSet << 30;
        if (pressed & secondMask) {
            result |= 0x80000000;
        }
        if (pressed & map->masks[2]) {
            result |= 0x10000000;
        }
        if (pressed & map->masks[3]) {
            result |= 0x20000000;
        }
        if (pressed & map->masks[4]) {
            result |= 0x08000000;
        }
        if (pressed & map->masks[6]) {
            result |= 0x02000000;
        }
        if (pressed & map->masks[5]) {
            result |= 0x04000000;
        }
        if (pressed & map->masks[7]) {
            result |= 0x01000000;
        }
        if (pressed & map->masks[13]) {
            result |= 0x00080000;
        }
        if (pressed & map->masks[12]) {
            result |= 0x00040000;
        }
        if (pressed & map->masks[8]) {
            result |= 0x00800000;
        }
        if (pressed & map->masks[9]) {
            result |= 0x00400000;
        }
        if (pressed & map->masks[11]) {
            result |= 0x00200000;
        }
        firstSet = pressed & map->masks[10];
        if (firstSet) {
            result |= 0x00100000;
        }
    }
    returnValue = result;
    return returnValue;
}
