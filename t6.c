#include "game/types.h"
extern s8 D_800A374D[];
extern s32 func_8001EF34(s32);
s32 h1(void) { return func_8001EF34((D_800A374D[0] ^ 3) == 0); }
s32 h2(void) { s32 v = D_800A374D[0]; return func_8001EF34((v ^ 3) == 0); }
