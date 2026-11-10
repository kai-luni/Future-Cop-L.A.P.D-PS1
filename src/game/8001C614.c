typedef unsigned char u8;
typedef int s32;

struct State {
    volatile s32 mode;
    char pad[24];
    s32 value;
};

extern struct State D_80010000;
extern s32 func_8008E194(s32 value);

void func_8001C614(u8 arg)
{
    switch (arg) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
        break;
    case 2:
        if (D_80010000.mode == 1) {
            D_80010000.mode = 2;
            func_8008E194(D_80010000.value);
        }
        break;
    }
}
