// Categories: Resources > Lookup
#include "game/types.h"

struct Unk51580Entry {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

struct Unk51580Header {
    s32 unk00;
    s32 unk04;
    u32 count;
};

struct Unk51580 {
    s32 unk00;
    u8 unk04[0x1C];
    s32 unk20;
};

struct Unk51580Out {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

extern struct Unk51580Header *func_80019A20(s32 magic, s32 arg1, s32 arg2);
extern void gVectorScaleInPlace(s32 arg0, struct Unk51580Out *out) __asm__("func_8001AB78");
s32 gGetLoopingPathSegmentDelta(struct Unk51580 *arg0, s32 arg1, struct Unk51580Out *arg2) __asm__("func_80051580");


/**
 *  @brief Looks up a one-based resource entry and populates an output record.
 *
 *  The resource header supplies the entry count, and a separate table
 *  supplies the selected entry and the value passed for further processing.
 *  @param arg0 Resource descriptor, or null to fail.
 *  @param arg1 One-based entry index.
 *  @param arg2 Output record, cleared on failure.
 *  @return 1 if the entry was processed, or 0 if the descriptor or index is invalid.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Checks the descriptor, count and positive index before accessing the table;
 *  otherwise clears all three output fields.
 *  @par Calls
 *  func_80019A20 retrieves the header and entry table; func_8001AB78 processes the output.
 *  @par Side effects
 *  Writes the output record and invokes the processing helper on success.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 gGetLoopingPathSegmentDelta(struct Unk51580 *arg0, s32 arg1, struct Unk51580Out *arg2)
{
    struct Unk51580Out *out;
    register struct Unk51580Entry *entry __asm__("$3");

    out = arg2;
    if (arg0 != 0) {
        if (func_80019A20(0x74415048, arg0->unk00, arg0->unk20)->count >= (u32)arg1) {
            if (arg1 > 0) {
                entry = (struct Unk51580Entry *)func_80019A20(0x61415048, arg0->unk00, arg0->unk20)
                        + (arg1 - 1);
                out->unk00 = entry->unk04;
                out->unk04 = 0;
                out->unk08 = entry->unk00;
                gVectorScaleInPlace(entry->unk08, out);
                return 1;
            }
        }
    }
    out->unk00 = 0;
    out->unk04 = 0;
    out->unk08 = 0;
    return 0;
}
