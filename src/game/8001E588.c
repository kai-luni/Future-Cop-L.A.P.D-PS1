// Categories: Input > Controller Mapping
#include "game/types.h"

struct PadMapping {
    u32 masks[14];
};

extern struct PadMapping gPadMappingTable[] __asm__("D_800AAC50");

/**
 *  @brief Initializes fourteen controller masks for a valid port.
 *
 *  Writes a fixed set of bit masks into the selected port's mapping table;
 *  ports outside the two-entry range are left unchanged.
 *  @param port Zero-based port index; only 0 and 1 are accepted.
 *  @par Globals
 *  D_800AAC50 contains the per-port mapping records written by this function.
 *  @par Control flow
 *  Checks port against 2 before assigning all fourteen masks.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Replaces the selected mapping's masks when the port is valid.
 *  @par Callers
 *  Confirmed: func_8001E4E4.
 */
void func_8001E588(u32 port)
{
    struct PadMapping *mapping;

    if (port < 2) {
        mapping = &gPadMappingTable[port];
        mapping->masks[0] = 0x100000;
        mapping->masks[1] = 0x400000;
        mapping->masks[2] = 0x800000;
        mapping->masks[3] = 0x200000;
        mapping->masks[4] = 0x80000000;
        mapping->masks[5] = 0x40000000;
        mapping->masks[6] = 0x20000000;
        mapping->masks[7] = 0x10000000;
        mapping->masks[8] = 0x8000000;
        mapping->masks[9] = 0x4000000;
        mapping->masks[10] = 0x1000000;
        mapping->masks[11] = 0x2000000;
        mapping->masks[12] = 0x10000;
        mapping->masks[13] = 0x80000;
    }
}
