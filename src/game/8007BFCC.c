// Categories: Game Engine > Front-End Configuration | Input > Port Presets
#include "game/types.h"

typedef struct {
    u8 unk00[8];
    s32 value;
} D_8009BEF4_Entry;

extern u8 gPresetIndexPerPort __asm__("D_8009FA98");
extern u8 gRumbleEnabled __asm__("D_8009F8D8");
extern u8 gPort2RumbleEnabled __asm__("D_8009F8D9");
extern u8 gStagedControllerPresets __asm__("D_8009F900");
extern u8 D_8009F901;
extern u8 gControllerDiagramViews __asm__("D_8009F902");
extern u8 D_8009F903;
extern s8 gSnapshotResult __asm__("D_8009F904");
extern u8 gPresetByteTable[] __asm__("D_8009BFD4");
extern D_8009BEF4_Entry gControllerPresetSlotTable[] __asm__("D_8009BEF4");
extern u8 gPadMappingTable[] __asm__("D_800AAC50");
extern u8 gPortCheckTable[] __asm__("D_800A374D");

extern u8 gIsPadRumbleEnabled(s32 arg0) __asm__("func_8001EB2C");

/**
 * @brief Applies one of four stored presets to a per-port configuration block.
 *
 * @param port  Port index, clamped to 0..1 (anything < 0 becomes 0, anything
 *              >= 2 becomes 1).
 * @param value Preset index, clamped to 0..3.
 *
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * - The preset index is stored in D_8009FA98[port] (addressed through
 *   D_8009FA99 - 1, the compiler's form for a two-byte array starting at
 *   0x8009FA98).
 * - src = D_8009BFD4 + value * 14: the four presets are 14 consecutive byte
 *   offsets each.
 * - The 14 words of D_8009BEF4 (stride 12: 8 unused bytes plus one value word)
 *   are written to D_800AAC50 + port * 56 + src[i] * 4, i.e. into the i-th
 *   slot named by the preset. With 14 slots of 4 bytes the target block is
 *   56 bytes per port.
 *
 * @par Likely role
 * A "copy the preset's values into the port's 14-slot configuration" helper.
 * The 2 x 56-byte layout and the two ports match the two controller/key
 * configuration blocks; the main loop applies preset 0 to both ports at boot.
 *
 * @par Globals
 * D_8009FA98 (preset index per port), D_8009BEF4 (value table),
 * D_8009BFD4 (4 x 14 offset table), D_800AAC50 (target blocks).
 * D_8009F8D8/D_8009F8D9/D_8009F900..D_8009F904 are declared but not touched
 * by this function (left over from an earlier revision of the unit).
 *
 * @par Callers
 * func_80076A2C (preset 0 for ports 0 and 1), func_8007C80C, func_80080984.
 */
void gApplyControllerPreset(s32 port, s32 value) __asm__("func_8007BFCC");

void gApplyControllerPreset(s32 port, s32 value)
{
    extern u8 gPort2ControllerPresetIndex[] __asm__("D_8009FA99");
    s32 clampedPort;
    s32 clampedValue;
    u8 *dst;
    u8 *src;
    s32 i;
    u8 *base;

    if (port >= 0) {
        if (port < 2) {
            clampedPort = port;
        } else {
            clampedPort = 1;
        }
    } else {
        clampedPort = 0;
    }
    port = clampedPort;

    if (value >= 0) {
        if (value < 4) {
            clampedValue = value;
        } else {
            clampedValue = 3;
        }
    } else {
        clampedValue = 0;
    }
    value = clampedValue;

    ((u8 *)gPort2ControllerPresetIndex - 1)[port] = value;
    src = gPresetByteTable + value * 14;
    i = 0;
    base = gPadMappingTable;
    dst = base + port * 56;
    while (i < 14) {
        *(s32 *)(dst + src[i] * 4) = gControllerPresetSlotTable[i].value;
        i++;
    }
}

