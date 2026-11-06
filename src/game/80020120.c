typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef struct {
    u8 pad[0x8C8];
    s32 count;
    u8 pad2[8];
    s32 h0, h1, h2, h3, h4, h5, h6, h7;
} State;
extern State *D_8009AB08;
extern u8 D_8001FAB8[], D_8001FF44[], D_8001FE40[], D_8001FD28[];
extern u8 D_8001F394[], D_8001F968[], D_8001F818[], D_8001F784[];
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern s32 func_8008C960(u32, u32, u32, void *);
extern void func_8008C980(s32);

void func_80020120(void)
{
    if (D_8009AB08->count == 0) {
        func_8008C9C0();
        D_8009AB08->h0 = func_8008C960(0xF4000001, 4, 0x1000, D_8001FAB8);
        D_8009AB08->h1 = func_8008C960(0xF4000001, 0x8000, 0x1000, D_8001FF44);
        D_8009AB08->h2 = func_8008C960(0xF4000001, 0x100, 0x1000, D_8001FE40);
        D_8009AB08->h3 = func_8008C960(0xF4000001, 0x2000, 0x1000, D_8001FD28);
        D_8009AB08->h4 = func_8008C960(0xF0000011, 4, 0x1000, D_8001F394);
        D_8009AB08->h5 = func_8008C960(0xF0000011, 0x8000, 0x1000, D_8001F968);
        D_8009AB08->h6 = func_8008C960(0xF0000011, 0x100, 0x1000, D_8001F818);
        D_8009AB08->h7 = func_8008C960(0xF0000011, 0x2000, 0x1000, D_8001F784);
        func_8008C980(D_8009AB08->h0);
        func_8008C980(D_8009AB08->h1);
        func_8008C980(D_8009AB08->h2);
        func_8008C980(D_8009AB08->h3);
        func_8008C980(D_8009AB08->h4);
        func_8008C980(D_8009AB08->h5);
        func_8008C980(D_8009AB08->h6);
        func_8008C980(D_8009AB08->h7);
        func_8008C9E0();
    }
    D_8009AB08->count++;
}
