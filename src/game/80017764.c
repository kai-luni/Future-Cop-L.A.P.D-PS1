// Categories: Graphics > Scene Graph > Transform Records | Resources > Memory Management
#include "game/types.h"

struct Unk17764 {
    u8 unk00[0x8C];
    struct Unk17764 *next;
};

extern struct Unk17764 *gCameraList __asm__("D_8009F588");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeCameraList(void) __asm__("func_80017764");


/**
 * @brief Releases every record of the transform list D_8009F588 and clears the
 *        list head.
 *
 * The list is a chain of 0x94-byte records built by func_80017994: 0x28 bytes
 * of header, two 12-word matrices copied from D_8009AA38, a serial id from
 * D_8009F58C, then next at +0x8C and previous at +0x90. The walk reads the
 * successor before releasing the current record, so the chain is freed in
 * place without a back pointer.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F588 - list head, set to 0 after the loop.
 * D_8009F58C - the id counter is deliberately not reset here; func_8001774C is
 * the function that clears both it and the head.
 *
 * @par Control flow
 * A single while loop over the chain: capture next, free the current record,
 * advance. The head is cleared once after the loop.
 *
 * @par Calls
 * func_800181D4 - the general free (pool, free-range or heap), one call per
 * record.
 *
 * @par Side effects
 * Frees every record and zeroes D_8009F588; the records become unreachable.
 *
 * @par Callers
 * Confirmed: func_80024D14 (the global subsystem reset, which runs it between
 * the scene teardown and the entity teardown).
 *
 * @par Environment path
 * Walks the camera record list D_8009F588 (0x94-byte records, next pointer at
 * +0x8C) and frees each record with gFreeBlock (func_800181D4), reading the
 * successor before the free, then sets the list head to 0. The id counter
 * D_8009F58C and the active camera pointer D_8009F9E4 are not touched here;
 * func_8001774C is the variant that clears all three. Its only caller is
 * gResetMissionSubsystems (func_80024D14), so it runs once during mission
 * teardown/reset, while records are created on demand by gCreateCamera
 * (func_80017994) from the actor reset func_800573A4.
 */
void gFreeCameraList(void)
{
    struct Unk17764 *node;
    struct Unk17764 *current;

    node = gCameraList;
    while (node != 0) {
        current = node;
        node = node->next;
        gFreeBlock(current);
    }
    gCameraList = 0;
}
