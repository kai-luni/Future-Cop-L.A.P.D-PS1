# 1 "t6.c"
# 1 "include/game/types.h" 1



typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

typedef char assert_s8_size[sizeof(s8) == 1 ? 1 : -1];
typedef char assert_u16_size[sizeof(u16) == 2 ? 1 : -1];
typedef char assert_u32_size[sizeof(u32) == 4 ? 1 : -1];


# 1 "t6.c" 2

extern s8 D_800A374D[];
extern s32 func_8001EF34(s32);
s32 h1(void) { return func_8001EF34((D_800A374D[0] ^ 3) == 0); }
s32 h2(void) { s32 v = D_800A374D[0]; return func_8001EF34((v ^ 3) == 0); }
