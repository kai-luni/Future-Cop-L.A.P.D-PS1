typedef unsigned int u32;

typedef struct DmaChannel {
    u32 madr;
    u32 bcr;
    u32 chcr;
    u32 pad;
} DmaChannel;

extern volatile u32 *D_8009DD60;
extern void (*D_8009DD64[])(void);
extern DmaChannel *D_8009DD84;

extern char D_8001286C[];
extern char D_80012888[];

extern void func_8008E000(char *, ...);

void func_80091D0C(void)
{
    int s0;
    u32 s1;

    s1 = (*D_8009DD60 >> 24) & 0x7F;
    while (s1 != 0) {
        for (s0 = 0; s1 != 0 && s0 < 7; s0++, s1 >>= 1) {
            if (s1 & 1) {
                *D_8009DD60 &= (1 << (s0 + 24)) | 0xFFFFFF;
                if (D_8009DD64[s0] != 0) {
                    D_8009DD64[s0]();
                }
            }
        }
        s1 = (*D_8009DD60 >> 24) & 0x7F;
    }

    if ((*D_8009DD60 & 0xFF000000) == 0x80000000 || (*D_8009DD60 & 0x8000)) {
        func_8008E000(D_8001286C, *D_8009DD60);
        for (s0 = 0; s0 < 7; s0++) {
            func_8008E000(D_80012888, s0, D_8009DD84[s0].madr);
        }
    }
    __asm__ volatile (".set reorder\n\t.set\tnoreorder");
}
