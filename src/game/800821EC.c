// Categories: Game Engine > State Transitions
#include "game/types.h"

struct State821EC {
    u8 pad0[0x200];
    u8 mode;
    u8 kind;
    u8 pad202[5];
    u8 flag;
    u8 pad208[0x73a];
    u16 value;
};

extern struct State821EC *gSaveProfileImage __asm__("D_8009F92C");
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;

/**
 *  @brief Advances selected modes in the current state object.
 *
 *  Mode 3 becomes 4 and updates two global bytes from the object's flag.
 *  Mode 5 becomes 2 and may reset the object's value.
 *  @par Globals
 *  D_8009F92C points to the state; D_8009F8BD and D_8009F8BE receive flag-derived values.
 *  @par Control flow
 *  Mode 3 clears the first byte and sets the second to the inverse of flag truth.
 *  Mode 5 sets value to 0x385e unless kind is 2 or 3; other modes do nothing.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the state object and, for mode 3, the two global bytes.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800821EC(void)
{
    if (gSaveProfileImage->mode == 3) {
        gSaveProfileImage->mode = 4;
        if (gSaveProfileImage->flag) {
            D_8009F8BD = 0;
            D_8009F8BE = 0;
        } else {
            D_8009F8BD = 0;
            D_8009F8BE = 1;
        }
    } else if (gSaveProfileImage->mode == 5) {
        gSaveProfileImage->mode = 2;
        if ((unsigned)(gSaveProfileImage->kind - 2) >= 2) {
            gSaveProfileImage->value = 0x385e;
        }
    }
}
