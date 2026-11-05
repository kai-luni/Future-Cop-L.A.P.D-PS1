#include "game/types.h"

struct Unk6BDFCCtl {
    u8 unk00[0x78];
    s32 unk78[3];
    s16 unk84[3];
    s16 unk8A[3];
    u8 unk90[3];
    u8 unk93[3];
};

struct Unk6BDFCObj {
    u8 unk00[0xAC];
    struct Unk6BDFCCtl *ctl;
};

struct Unk6BDFCTgt {
    u8 unk00[0x2A];
    u8 unk2A;
    u8 unk2B[0x19];
    s32 unk44;
    u8 unk48[8];
    s32 unk50;
};

extern struct Unk6BDFCTgt *func_8006AAEC(u8 id);
extern s32 func_8006BD48(struct Unk6BDFCObj *obj, struct Unk6BDFCTgt *tgt, s32 flag);

#define U8(off)  (*(u8 *)((u32)ctl + idx + (off)))
#define S16(off) (*(s16 *)((u32)ctl + (idx << 1) + (off)))

void func_8006BDFC(struct Unk6BDFCObj *obj, s32 idx)
{
    struct Unk6BDFCCtl *ctl = obj->ctl;
    struct Unk6BDFCTgt *tgt = func_8006AAEC(U8(0x90));
    s16 val = S16(0x8A);
    s32 t;
    s32 t2;

    if (val <= 0) {
        U8(0x93) = tgt->unk2A;
        t = tgt->unk50;
        t2 = tgt->unk44;
        tgt->unk44 = t;
        tgt->unk50 = t2;
    }
    S16(0x8A) = func_8006BD48(obj, tgt, 1);
}
