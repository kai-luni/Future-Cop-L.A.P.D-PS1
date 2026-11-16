// Categories: Graphics > Terrain | Game Engine > Init
typedef unsigned char u8;
typedef signed int s32;

extern u8 D_800A2730[];
extern u8 D_800A6F38[];
extern s32 D_800A8E80[];

extern s32 D_8009F80C;
extern s32 D_8009F810;
extern s32 D_8009FA04;
extern int *D_8009F808;
extern int *D_8009FA08;

extern void func_80063134(void);
extern void *func_80017EBC(s32 size, s32 mode);
extern int func_80018460(int *value);
extern void func_8008CD20(void *ptr, s32 arg1, s32 arg2);

/**
 * @brief Clears map/terrain tables and allocates the tile and mesh pointer buffers.
 *
 * Zeroes 0x201 bytes at D_800A2730, calls func_80063134, clears D_8009F80C and D_8009F810,
 * allocates and zeroes D_8009F808 (0x1A0 B) and D_8009FA08 (0x1B18 B), and resets D_800A8E80[0..9], D_8009FA04, and D_800A6F38[0].
 *
 * @param void No parameters.
 * @return Pointer to the newly allocated and zeroed D_8009FA08 buffer.
 *
 * @par Globals
 * D_800A2730 (0x201 bytes cleared), D_8009F808/D_8009FA08 (tag-2 buffers),
 * D_8009F80C, D_8009F810, D_8009FA04, D_800A8E80[0..9], D_800A6F38[0] (all cleared).
 * @par Control flow
 * Backwards 0x201-byte clear loop, straight-line resets/allocations using func_80018460 size headers, and 10-entry reverse zero loop.
 * @par Calls
 * func_80063134 (clears map state), func_80017EBC (allocates tag-2 blocks), func_80018460 (reads block size), func_8008CD20 (memset zero).
 * @par Side effects
 * Allocates two tag-2 heap buffers and resets terrain/texture globals.
 * @par Callers
 * Confirmed: func_80024A24 (mission boot).
 */
int *func_80063428(void)
{
    s32 i;

    {
        register s32 count __asm__("$3");
        register u8 *cursor __asm__("$2");

        count = 0x200;
        cursor = D_800A2730;
        cursor += 0x200;
        do {
            *cursor = 0;
            --count;
            --cursor;
        } while (count >= 0);
    }

    func_80063134();
    D_8009F80C = 0;
    D_8009F810 = 0;
    D_8009F808 = func_80017EBC(0x1A0, 2);
    func_8008CD20(D_8009F808, 0, func_80018460(D_8009F808));
    D_8009FA08 = func_80017EBC(0x1B18, 2);
    func_8008CD20(D_8009FA08, 0, func_80018460(D_8009FA08));

    for (i = 9; i >= 0; i--) {
        D_800A8E80[i] = 0;
    }

    {
        register int *result __asm__("$2");

        result = D_8009FA08;
        D_8009FA04 = 0;
        D_800A6F38[0] = 0;
        return result;
    }
}
