// Categories: Platform > System Init | Game Engine > Boot | Resources | Graphics
#include "game/types.h"

extern u32 D_8009EA04;
extern u32 D_8009EA08;
extern u32 *gGameSession __asm__("D_8009FA94");
extern u32 *D_8009FAD4;
extern u32 *gVmCountdowns __asm__("D_8009F9A4");
extern u32 *gOpcodeTable __asm__("D_8009F9F8");
extern u8 D_8009B2DC[];

extern void func_8008CA30(u32 arg0, u32 arg1);
extern u32 *func_8001E240(s32 arg0, s32 arg1);
extern void func_80017BE8(void);
extern void func_8008DEA0(void);
extern s32 func_8001E4E4(void);
extern s32 func_8001BF78(void);
extern void func_8001C088(s32 mode, s32 enable);
extern void func_8001C040(u32 color);
extern s32 func_8001BFE4(void);
extern void func_8001D248(s32 arg0, s32 arg1, u8 *arg2);
extern void func_8001F0B8(void);
extern void func_8002333C(void);

/**
 * @brief Brings the runtime up: heap, device layer, state block, CD-ROM and
 *        the fade layers. Called once from the main loop.
 *
 * @return 1 when the runtime is fully initialised, 0 when one of the two
 *         capability checks failed (the caller then skips the game loop and
 *         shuts down again).
 *
 * @par Confirmed flow
 * 1. func_8008CA30(D_8009EA04, (D_8009EA08 >> 3) << 3) - hands the free memory
 *    range computed by the entrypoint to the low-level block allocator
 *    (writes the block header, stores the base in D_8009C7D0 and the word
 *    count in the block).
 * 2. D_8009FA94 = func_8001E240(0xF4, 3) - the state block (see
 *    func_80076A2C), then 0x39 words of it are zeroed (i = 0x38 .. 0, i.e.
 *    0xE4 of the 0xF4 bytes; the last 0x10 bytes are left untouched).
 * 3. D_8009FAD4 = 0x190 bytes, D_8009F9A4 and D_8009F9F8 = 0x80 bytes each -
 *    three more work buffers.
 * 4. func_80017BE8() initialises the heap (range table + 0x15A000-byte block),
 *    func_8008DEA0() initialises the CD-ROM subsystem (its failure string is
 *    "CdInit: Init failed\n").
 * 5. Two capability checks, func_8001E4E4() and func_8001BF78(); a non-zero
 *    result means "cannot initialise" and returns 0.
 * 6. func_8001C088(1, 1) + func_8001C040(0) and func_8001C088(0, 1) +
 *    func_8001C040(0) select both fade layers and set them to black, so the
 *    first visible frame is guaranteed to be black.
 * 7. func_8001BFE4(), func_8001D248(0, 0, D_8009B2DC), func_8001F0B8(),
 *    func_8002333C() start the per-frame subsystem (default callbacks, frame
 *    counter, callback table registration and the 8-slot object table).
 * 8. Returns 1.
 *
 * @par Globals
 * D_8009FA94 (state block), D_8009FAD4/D_8009F9A4/D_8009F9F8 (buffers),
 * D_8009EA04/D_8009EA08 (heap window from the entrypoint), D_8009B2DC (table
 * handed to func_8001D248).
 *
 * @par Likely details
 * func_8001E4E4() sets up the pad/graphics device layer (it clears
 * D_8009FAD8/D_8009FA20 and calls func_8001E588(port) for ports 0 and 1) and
 * func_8001BF78() initialises two fade layers (D_8009F225 = 2, then the
 * D_800A06E0 array). Their return values are always 0 in this build, so the
 * failure path exists only as a guard.
 *
 * @par Unknown
 * The roles of D_8009FAD4, D_8009F9A4 and D_8009F9F8 (0x190/0x80/0x80 bytes)
 * are not visible from the call graph; they are only freed by func_80076FE0.
 */
s32 func_80076EB0(void)
{
    register s32 result __asm__("$16");
    s32 i;

    result = 0;
    func_8008CA30(D_8009EA04, (D_8009EA08 >> 3) << 3);

    gGameSession = func_8001E240(0xF4, 3);
    for (i = 0x38; i >= 0; i--) {
        gGameSession[i] = 0;
    }

    D_8009FAD4 = func_8001E240(0x190, 3);
    gVmCountdowns = func_8001E240(0x80, 3);
    gOpcodeTable = func_8001E240(0x80, 3);

    func_80017BE8();
    func_8008DEA0();

    if (func_8001E4E4() != 0) {
        goto done;
    }
    if (func_8001BF78() != 0) {
        goto done;
    }

    func_8001C088(1, 1);
    func_8001C040(0);
    func_8001C088(0, 1);
    func_8001C040(0);

    result = 1;
    func_8001BFE4();
    func_8001D248(0, 0, D_8009B2DC);
    func_8001F0B8();
    func_8002333C();

done:
    return result;
}
