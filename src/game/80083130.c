#include "game/types.h"

typedef struct {
    char *unk0;
    s32 unk4;
    s32 unk8;
    u8 pad0C[0x15];
    u8 unk21;
    u8 pad22[2];
} Entry;

extern u8 D_8009B300[];
extern u8 D_8009F488[];
extern u32 D_8009F444[];
extern u8 D_80011E60[];
extern Entry D_8009B4E4[];

extern s32 func_8008CCC0(u8 *);
extern u8 func_8008CCF0(u8);
extern s32 func_8008CCA0(void *, void *);

s32 func_80083130(void **arg0)
{
    register void **arg0_reg __asm__("$20") = arg0;
    register s32 res __asm__("$19");
    register u32 len __asm__("$16");
    u32 i;
    char buffer[24];
    u8 c;
    void *s2;
    Entry *entry;

    len = func_8008CCC0(D_8009B300);
    i = 0;
    res = 0;
    if (len != 0) {
        do {
            c = func_8008CCF0(D_8009B300[i]);
            buffer[i] = c;
            if (c == 0x2E) {
                break;
            }
            i++;
        } while (i < len);
    }
    buffer[i] = 0;

    if (func_8008CCA0(buffer, D_8009F488) == 0) {
        __builtin_memcpy((u32 *)buffer, D_8009F444, 5);
    }

    i = 0;
    s2 = D_80011E60;
    for (; i < 16; i++) {
        entry = &D_8009B4E4[i];
        if (func_8008CCA0(buffer, entry->unk0) == 0) {
            res = entry->unk8;
            if (arg0_reg == 0) {
                return res;
            }
            if (entry->unk21 != 0) {
                *arg0_reg = s2;
            } else {
                *arg0_reg = 0;
            }
            break;
        }
    }

    return res;
}
