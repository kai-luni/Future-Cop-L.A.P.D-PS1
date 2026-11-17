// Categories: Resources > Movie Playback | Graphics > Frame State
#include "game/types.h"

extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern u8 *gSaveProfileImage __asm__("D_8009F92C");

extern void func_8007FA88(void);
extern void func_8007F8E0(void);
extern void func_800776B8(void);

/**
 * @brief Initialises the movie-playing front-end screen: re-arms playback,
 *        clears the frame state and resets the menu list.
 *
 * Registered as the init callback (+0x10) of the screen descriptor at
 * D_800A36B8, next to the update callback func_8007FACC (+0x14) and the draw
 * callback D_80080038 (+0x18). It arms the next movie, resets the frame state,
 * clears the two pending bytes and the two tail fields of the 0x948-byte movie
 * context, and finally resets the front-end list cursor.
 *
 * @param void No parameters; every input is a global.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F92C - movie context (0x948 bytes, allocated by func_8007F9CC); the
 * 16-bit field at +0x942 and the byte at +0x944 are cleared here.
 * D_8009F8BD/D_8009F8BE - the two "pending" bytes that func_8007FACC sets and
 * consumes; both are zeroed here (gp-relative stores in retail).
 *
 * @par Control flow
 * Linear: arm, reset, four stores, reset the list. No branches.
 *
 * @par Calls
 * func_8007FA88 - resets the decoder and requests the start of the next movie.
 * func_8007F8E0 - clears the context state byte +0x921 and both pending bytes.
 * func_800776B8 - resets the front-end list: cursor 0, list D_8009B724, count 8.
 *
 * @par Side effects
 * Writes the two flags, three context fields and the front-end list state.
 *
 * @par Callers
 * Unknown: no direct call. The address is stored at D_800A36B8+0x10 (lui/addiu
 * at 0x80077EF0), so it is reached through the front-end screen table.
 */
void func_8007F8FC(void)
{
    u8 *p;

    func_8007FA88();
    func_8007F8E0();
    p = gSaveProfileImage;
    gUiHoldFlag = 0;
    gUiInteractionEnabled = 0;
    *(u16 *)(p + 0x942) = 0;
    *(u8 *)(p + 0x944) = 0;
    func_800776B8();
}
