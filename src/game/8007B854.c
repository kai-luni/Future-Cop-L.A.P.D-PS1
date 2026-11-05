#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    u8 pad8[4];
    s16 unkC;
    u8 unkE;
    u8 padF[0x10];
    u8 unk1F;
    u8 pad20[4];
} Entry;
typedef struct {
    u8 pad0[0xC];
    s32 unkC;
    u8 pad10[0x3C];
    s32 unk4C;
    u8 pad50[0xC];
    s32 unk5C;
} State;
extern State *D_8009FA94;
extern Entry D_8009B4E4[];
extern s8 D_8009F8C0[];
extern s32 D_8009F8B8;
extern s8 D_8009F87E, D_8009F87F, D_8009F880, D_8009F881, D_8009F882;
extern s8 *D_8009F884;
extern u8 D_8009F8BD;
extern s32 D_8009F8C4, D_8009F8C8;
extern u8 D_8009B300[], D_8009F444[], D_8009F488[];
extern u8 D_800A3628[], D_800A3658[], D_800A3838[];
extern u8 *D_8009F3EC;
extern u8 D_8009F3F0;
extern u8 *D_8009F468;
extern void func_80078650(void *, s32);
extern s32 func_80046C84(s32, s32);
extern void func_8008CCB0(void *, s32);
extern s32 func_8008CCA0(void *, void *);
extern void func_80078B34(s32, s32, s32);
extern void func_8007B460(s32);
#define SELECTED (D_8009B4E4[D_8009F884[D_8009F880]])

void func_8007B854(s32 arg0)
{
    State *state;
    u8 special;
    u8 ready;
    register s8 *p __asm__("$6");
    register void *message __asm__("$4");
    register s8 *base __asm__("$16");
    register s32 index __asm__("$2");
    register s32 firstIndex __asm__("$3");
    u8 limit;
    register s32 value __asm__("$2");
    register Entry *sel __asm__("$4");
    register s32 scaled __asm__("$2");
    register Entry *ep __asm__("$3");
    switch (arg0) {
    case 3:
        firstIndex = D_8009F8B8;
        __asm__("" : "+r"(firstIndex));
        p = &D_8009F8C0[firstIndex];
        special = 0;
        if (*p != 0) {
            ready = 0;
            state = D_8009FA94;
            if (state->unk5C == 1) {
                if (state->unkC == 1) {
                    if (*p == 1) {
                        sel = D_8009B4E4;
                        scaled = D_8009F884[D_8009F880] * 36;
                        __asm__("addu %0, %1, %2" : "=r"(ep) : "r"(scaled), "r"(sel));
                        value = ep->unk1F;
                        if (value != 0) {
                            *p = 2;
                            limit = SELECTED.unk1F;
                            D_8009F87F = limit;
                            if ((s8)limit >= 0) {
                                if ((s8)limit < 10) goto CLAMPED;
                                limit = 9;
                            } else {
                                limit = 0;
                            }
                            D_8009F87F = limit;
CLAMPED:
                            D_8009F87E = limit;
                            D_8009F8BD = 0;
                            goto SELECTION_DONE;
                        }
                    } else {
                        value = D_8009F87E;
                    }
                    ready = 1;
                    __asm__("" : "+r"(ready));
                    state->unk4C = value;
SELECTION_DONE:;
                } else {
                    ready = 1;
                }
                if (ready) {
                    if (D_8009FA94->unkC == 1) message = D_800A3628;
                    else message = D_800A3658;
                    func_80078650(message, 1);
                    D_8009F8C8 = 2;
                    D_8009F8C4 = 0;
                    if (SELECTED.unk1F == 10) special = SELECTED.unkE;
                }
            } else {
                D_8009F8C8 = 1;
                D_8009F8C4 = SELECTED.unk4;
            }
            if (D_8009F8C8 != 0) {
                func_8008CCB0(D_8009B300, SELECTED.unk0);
                if (func_8008CCA0(D_8009B300, D_8009F444) == 0) D_8009F3EC = D_8009F488;
                else D_8009F3EC = 0;
                if (special != 0) D_8009F3F0 = special;
                else if (SELECTED.unkC != 0) D_8009F3F0 = SELECTED.unkC;
            }
        } else {
            *p = 1;
        }
        break;
    case 2:
    case 4:
        index = D_8009F8B8;
        __asm__("" : "+r"(index));
        base = D_8009F8C0;
        if (base[index] != 0) {
            func_80046C84(0x46, 0);
            if (base[D_8009F8B8] == 2) {
                D_8009F87E--;
                if (D_8009F87E < 0) D_8009F87E = D_8009F87F;
            } else {
                D_8009F880--;
                if (D_8009F880 < D_8009F881) D_8009F880 = D_8009F882;
            }
        }
        break;
    case 1:
    case 5:
        index = D_8009F8B8;
        __asm__("" : "+r"(index));
        base = D_8009F8C0;
        if (base[index] != 0) {
            func_80046C84(0x46, 0);
            if (base[D_8009F8B8] == 2) {
                D_8009F87E++;
                if (D_8009F87E > D_8009F87F) D_8009F87E = 0;
            } else {
                D_8009F880++;
                if (D_8009F880 > D_8009F882) D_8009F880 = D_8009F881;
            }
        }
        break;
    case 0:
        if (D_8009F468 == D_800A3838) {
            func_80078B34(0x44C, 0, 0);
            func_80078B34(0x10CC, 1, 0);
            func_8007B460(D_8009F8B8);
        }
        break;
    }
}
