// Categories: Game Engine > State Serialization
#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 pad[4];
};

struct UnkFA94 {
    u8 pad00[0x8C];
    u8 unk8C;
    u8 pad8D[3];
    u8 unk90;
    u8 pad91[3];
    u8 unk94;
};

extern struct Entry gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gFrontEndListTable[] __asm__("D_8009B72C");
extern struct UnkFA94 *gGameSession __asm__("D_8009FA94");
extern u8 gFrontEndListFlag __asm__("D_8009F3C0");
extern u8 gPlayerWrapLimit __asm__("D_8009F3C1");
extern u8 gPausedMenuFlag __asm__("D_8009F3C3");
extern u8 gLockedMenuFlag __asm__("D_8009F3C4");
extern u8 gCrimeWarProgress __asm__("D_8009F458");
extern u8 D_800A3938[];

extern void gPackInputRecord(u8 *output, u32 value1, u32 value2, u32 value3,
                          u32 value4, u32 value5, u32 value6, u32 value7,
                          u32 value8, u32 value9, u32 value10, u32 value11,
                          u32 value12, u32 value13) __asm__("func_8007E8D8");

/**
 *  @brief Pass current mode and selection values to an output builder.
 *
 *  Collects three mode bytes, several standalone state bytes, and five bytes
 *  selected through an index table, preserving their argument order.
 *  @par Globals
 *  D_8009FA94 supplies three mode bytes; D_8009F3C0, D_8009F3C1,
 *  D_8009F3C3, D_8009F3C4, and D_8009F458 supply state bytes.
 *  D_8009B72C indexes D_8009B4E4; D_800A3938 is the output buffer.
 *  @par Control flow
 *  Performs one unconditional call with the collected values.
 *  @par Calls
 *  func_8007E8D8 receives the output buffer and thirteen values.
 *  @par Side effects
 *  Passes D_800A3938 to the builder for output; its writes are not shown here.
 *  @par Callers
 *  Confirmed: func_8007E7A8, func_8007F93C.
 */
void func_8007F02C(void)
{
    gPackInputRecord(D_800A3938,
                  gGameSession->unk8C,
                  gGameSession->unk90,
                  gGameSession->unk94,
                  gFrontEndListFlag,
                  gCrimeWarProgress,
                  gWideTextTable[gFrontEndListTable[0]].unk1F,
                  gWideTextTable[gFrontEndListTable[1]].unk1F,
                  gWideTextTable[gFrontEndListTable[2]].unk1F,
                  gWideTextTable[gFrontEndListTable[3]].unk1F,
                  gPlayerWrapLimit,
                  gPausedMenuFlag,
                  gWideTextTable[gFrontEndListTable[4]].unk1F,
                  gLockedMenuFlag);
}
