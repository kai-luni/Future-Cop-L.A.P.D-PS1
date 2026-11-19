// Categories: Platform > CD-ROM | Resources > Device Init
#include "game/types.h"

extern u8 gMediaBypassFlag __asm__("D_8009F234");
extern s32 gFreeCallback __asm__("D_8009F22C");
extern s32 gAllocCallback __asm__("D_8009F230");
extern void *gFileSearchMgr __asm__("D_8009F240");

extern void D_8001CDD0(void);
extern void D_8001CDF0(void);
extern u8 gDefaultSearchMgr[] __asm__("D_8009AABC");
extern s32 gFileSlotTable[] __asm__("D_800A6D78");
extern s32 gCdDriveStateTable[] __asm__("D_800A8E68");
extern u32 jtbl_80010AA4[];
extern s32 gProbeCdDriveMode(s32 arg0) __asm__("func_8001D130");
s32 gInitCdFileSystem(s32 arg0, s32 arg1, u8 *arg2) __asm__("func_8001D248");


/**
 * @brief Initialises the file/CD layer: installs the allocator callbacks,
 *        clears the four file slots and probes the drive/media slots.
 *
 * @param arg0 Optional allocation callback; both arguments must be given or
 *             both omitted.
 * @param arg1 Optional free callback (same rule as arg0).
 * @param arg2 Optional pointer stored in D_8009F240; when NULL the default
 *             D_8009AABC is used.
 *
 * @return 0 on success, -0x11 when only one of arg0/arg1 is non-zero, -7 when
 *         none of the four probed slots reported a usable state.
 *
 * @par Confirmed flow
 * 1. With arg0 == arg1 == 0 the defaults D_8001CDD0 (alloc) and D_8001CDF0
 *    (free) are installed in D_8009F230/D_8009F22C; otherwise both given
 *    pointers are stored.
 * 2. The four file slots D_800A6D78 (0x18 bytes apart) are cleared word-wise.
 * 3. For i = 1..3: idx = func_8001D130(i); the result shifted by 0x10 indexes
 *    the jump table jtbl_80010AA4 (four cases: 0, 3, 1, default). Case 0 and
 *    case 3 store 0 and 3 and count as "usable"; case 1 stores 1; anything else
 *    stores 2 into D_800A8E68[i].
 * 4. count == 0 means no usable slot: -7.
 * 5. D_8009F234 = 1 ("file layer ready") and D_8009F240 = arg2 (or
 *    D_8009AABC), return 0.
 *
 * @par Likely role
 * A Sony-style device/drive probe: the per-index state codes (0/1/2/3) look
 * like "no media / ready / not present / changed" and D_8009F234 is the
 * "initialised" flag read elsewhere.
 *
 * @par Globals
 * D_8009F230/D_8009F22C (alloc/free callbacks), D_8009F234 (ready flag),
 * D_8009F240 (private pointer), D_800A6D78 (file slots), D_800A8E68 (per-slot
 * state), jtbl_80010AA4.
 *
 * @par Callers
 * func_80076EB0 (boot) and func_8001CE10.
 *
 * @par Role and path
 * Installs allocator callbacks into gAllocCallback (D_8009F230) and
 * gFreeCallback (D_8009F22C), clears all four slots in gFileSlotTable
 * (D_800A6D78), and probes modes 1..3 via func_8001D130 into
 * gCdDriveStateTable[1..3]. Sets gMediaBypassFlag (D_8009F234) to 1 and
 * stores arg2 or gDefaultSearchMgr into gFileSearchMgr; called by
 * func_80076EB0 and func_8001CE10.
 */
s32 gInitCdFileSystem(s32 arg0, s32 arg1, u8 *arg2)
{
    register s32 count __asm__("$18");
    u8 *arg2_reg;
    register s32 i __asm__("$16");
    register s32 *entry __asm__("$17");
    register void **table __asm__("$19");
    register s32 idx __asm__("$3");
    register s32 v0 __asm__("$2");
    s32 *clear;
    static void *const label_table[] __attribute__((used)) = {
        &&case_zero, &&case_three, &&case_one, &&default_case
    };

    arg2_reg = arg2;
    count = 0;

    if (arg0 != 0) {
        if (arg1 != 0) {
            goto have_args;
        }
        return -0x11;
    }
    if (arg1 != 0) {
        return -0x11;
    }
    gAllocCallback = (s32)&D_8001CDD0;
    gFreeCallback = (s32)&D_8001CDF0;
    goto cleared;
have_args:
    gAllocCallback = arg0;
    gFreeCallback = arg1;
cleared:

    i = 0;
    clear = (s32 *)gFileSlotTable;
    for (; i < 4; i++) {
        *clear = 0;
        clear = (s32 *)((u8 *)clear + 0x18);
    }

    i = 1;
    v0 = 0x80010000;
    __asm__("" : "+r"(v0));
    table = (void **)(v0 + 0xAA4);
    v0 = (s32)gCdDriveStateTable;
    entry = (s32 *)(v0 + 4);

loop:
    idx = gProbeCdDriveMode(i) + 0x10;
    if ((u32)idx >= 0x11) {
        goto default_case;
    }
    v0 = idx;
    v0 = v0 << 2;
    v0 = v0 + (s32)table;
    v0 = *(s32 *)v0;
    goto *(void *)v0;

case_zero:
    *entry = 0;
    count++;
    goto next;

case_three:
    *entry = 3;
    count++;
    goto next;

case_one:
    v0 = 1;
    goto store;

default_case:
    v0 = 2;

store:
    *entry = v0;

next:
    i++;
    entry++;
    if (i < 4) {
        goto loop;
    }

    if (count == 0) {
        return -7;
    }
    gMediaBypassFlag = 1;
    if (arg2_reg == 0) {
        gFileSearchMgr = gDefaultSearchMgr;
        return 0;
    }
    gFileSearchMgr = arg2_reg;
    return 0;
}
