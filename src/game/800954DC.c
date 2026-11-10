__asm__(".section .text.target,\"ax\",@progbits\n\t.balign 4");
typedef unsigned int u32;

typedef struct Entry {
    u32 data;
    short width;
    short height;
} Entry;

typedef struct Callbacks {
    void *pad[7];
    void (*write)(void *, void *);
} Callbacks;

extern char D_80012AA0[];
extern volatile u32 *D_8009DED4, *D_8009DEC8;
extern u32 D_8009DEFC, D_8009DF00;
extern Callbacks *D_8009DDE8;
extern int func_80091270(int);
extern void func_80092CF4(char *, void *);
extern int func_8009520C(void);
extern void func_800914E0(int, void (*)(void));
extern void D_80095808(void);

int func_800954DC(Entry *entry, void *data)
{
    int result;
    register char *first __asm__("$4");
    __asm__("lui %0,%%hi(D_80012AA0)\n\taddiu %0,%0,%%lo(D_80012AA0)" : "=r"(first));
    func_80092CF4(first, entry);
    D_8009DEFC = func_80091270(-1) + 0xF0;
    D_8009DF00 = 0;
    while ((*D_8009DED4 & 0x01000000) || !(*D_8009DEC8 & 0x04000000)) {
        if (func_8009520C()) { result = -1; goto done; }
    }
    {
        register void (*callback)(void) __asm__("$5");
        __asm__("lui %0,%%hi(D_80095808)\n\taddiu %0,%0,%%lo(D_80095808)" : "=r"(callback));
        func_800914E0(2, callback);
    }
    D_8009DDE8->write(entry, data);
    result = 0;
done:
    __asm__ volatile(".set reorder");
    return result;
}
