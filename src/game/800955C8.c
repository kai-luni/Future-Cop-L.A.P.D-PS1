// Categories: Platform > GPU | Graphics > GPU > Debug Validation
#include "game/gpu_globals.h"

__asm__(".section .text.target,\"ax\",@progbits\n\t.balign 4");
typedef unsigned int u32;

typedef struct Entry {
    u32 data;
    short width;
    short height;
} Entry;

typedef struct Callbacks {
    void *pad[6];
    void (*unk18)(void *);
} Callbacks;

extern char D_80012AAC[];
extern volatile u32 *D_8009DED4;
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
extern u32 D_8009DEFC, D_8009DF00;
extern Callbacks *D_8009DDE8;
extern u32 D_8009DE90[];
extern int func_80091270(int);
extern void func_80092CF4(char *, void *);
extern int func_8009520C(void);
extern void func_800914E0(int, void (*)(void));
extern void D_80095808(void);

/**
 * @brief Debug capture variant C: waits for the GPU, packs a three-word
 *        capture descriptor and hands it to the device callback.
 *
 * @param entry Debug entry {data, width, height}; width/height must be non-zero.
 * @param a, b Packed into the second descriptor word (b << 16 | a).
 * @return 0 on success, -1 on abort or a zero dimension.
 *
 * @par Confirmed
 * Same format/wait sequence as func_800954DC (format string D_80012AAC,
 * deadline func_80091270(-1) + 0xF0, GPU status poll with func_8009520C
 * abort, func_800914E0(2, D_80095808) callback install). Then the buffer at
 * D_8009DE90 receives {entry->data, (b << 16) | (a & 0xFFFF), width/height
 * word} and D_8009DDE8->unk18(buf - 2) is called.
 *
 * @par Likely role
 * The rectangular-capture variant of the debug device: unlike the write
 * variants it passes a descriptor block instead of streaming the entry.
 *
 * @par Unknown
 * Why the callback receives buf - 2 (a header before the descriptor).
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; likely an unused retail variant.
 */
int func_800955C8(Entry *entry, u32 a, u32 b)
{
    int result;
    register char *first __asm__("$4");
    __asm__("lui %0,%%hi(D_80012AAC)\n\taddiu %0,%0,%%lo(D_80012AAC)" : "=r"(first));
    func_80092CF4(first, entry);
    D_8009DEFC = func_80091270(-1) + 0xF0;
    D_8009DF00 = 0;
    while ((*D_8009DED4 & 0x01000000) || !(*gGpuGp1PortPtr & 0x04000000)) {
        if (func_8009520C()) { result = -1; goto done; }
    }
    {
        register void (*callback)(void) __asm__("$5");
        __asm__("lui %0,%%hi(D_80095808)\n\taddiu %0,%0,%%lo(D_80095808)" : "=r"(callback));
        func_800914E0(2, callback);
    }
    if (entry->width == 0) { result = -1; goto done; }
    if (entry->height == 0) { __asm__ volatile(""); result = -1; goto done; }
    {
            register u32 *buf __asm__("$4");
            u32 v = (b << 16) | (a & 0xFFFF);
            register u32 d __asm__("$5") = entry->data;
            Callbacks *cb = D_8009DDE8;
            __asm__("lui %0,%%hi(D_8009DE90)\n\taddiu %0,%0,%%lo(D_8009DE90)" : "=r"(buf) : "r"(v), "r"(cb));
            buf[1] = v;
            buf[0] = d;
            buf[2] = *(u32 *)&entry->width;
            cb->unk18(buf - 2);
        result = 0;
    }
done:
    __asm__ volatile(".set reorder");
    return result;
}
