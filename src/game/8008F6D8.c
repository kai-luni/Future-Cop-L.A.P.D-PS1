typedef unsigned char u8;
typedef unsigned int u32;
extern volatile u8 *D_8009CB6C, *D_8009CB78, *D_8009CB74;
extern u32 *D_8009CB7C;
extern volatile u8 D_8009CB84[];

void func_8008F6D8(void)
{
    *D_8009CB6C = 1;
    while (*D_8009CB78 & 7) {
        *D_8009CB6C = 1;
        *D_8009CB78 = 7;
        *D_8009CB74 = 7;
    }
    {
        register volatile u8 *state __asm__("$3") = D_8009CB84;
        register volatile u8 *port __asm__("$4");
        state[2] = 0;
        state[1] = state[2];
        port = D_8009CB6C;
        __asm__("" : "+r"(state), "+r"(port));
        state[0] = 2;
        *port = 0;
    }
    *D_8009CB78 = 0;
    *D_8009CB7C = 0x1325;
}
