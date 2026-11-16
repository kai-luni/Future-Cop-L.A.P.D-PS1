// Categories: Graphics > Scene Graph > Transform Records | Resources > Memory Management
#include "game/types.h"

struct Unk17764 {
    u8 unk00[0x8C];
    struct Unk17764 *next;
};

extern struct Unk17764 *D_8009F588;

extern void func_800181D4(void *block);

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
 */
void func_80017764(void)
{
    struct Unk17764 *node;
    struct Unk17764 *current;

    node = D_8009F588;
    while (node != 0) {
        current = node;
        node = node->next;
        func_800181D4(current);
    }
    D_8009F588 = 0;
}
