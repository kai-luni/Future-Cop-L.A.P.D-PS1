// Categories: Platform > Sound | Game Engine > Init
#include "game/types.h"

extern int gSpuSetCommonAttributes() __asm__("func_80098FE8");

struct Unk21824 {
    u32 f0;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fA;
    u16 fC;
    u16 fE;
    u16 f10;
    u16 f12;
    u32 f14;
    u32 f18;
    u16 f1C;
    u16 f1E;
    u32 f20;
    u32 f24;
};

void gInitSpuCommonAttributes(void) __asm__("func_80021824");

/**
 * @brief Initializes the common SPU attributes with maximum master-volume
 * values.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Builds a 0x28-byte stack record with mask 0x3FC3, left and right volume
 * halfwords of 0x3FFF, and zeroes in all remaining fields. Passes the record
 * to gSpuSetCommonAttributes.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gResetCvkSubsystem.
 *
 * @par Role and path
 * Sound system startup configures the shared SPU volume and mix state through
 * this call.
 *
 * @par Unknown
 * The callee interprets each mask bit against the common SPU attribute layout.
 */
void gInitSpuCommonAttributes(void)
{
    struct Unk21824 attr;

    attr.f0 = 0x3FC3;
    attr.f4 = 0x3FFF;
    attr.f6 = 0x3FFF;
    attr.f8 = 0;
    attr.fA = 0;
    attr.fC = 0;
    attr.fE = 0;
    attr.f10 = 0;
    attr.f12 = 0;
    attr.f14 = 0;
    attr.f18 = 0;
    attr.f1C = 0;
    attr.f1E = 0;
    attr.f20 = 0;
    attr.f24 = 0;
    gSpuSetCommonAttributes(&attr);
}
