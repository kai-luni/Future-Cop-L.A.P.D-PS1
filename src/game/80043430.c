#include "game/types.h"

typedef struct Node43430 {
    struct Node43430 *next;
    u8 unk04[0x18];
} Node43430;

typedef struct {
    u8 unk00[0x64];
    void *unk64;
    s32 unk68;
    void *unk6C;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    s16 unk76;
    s16 unk78;
    s16 unk7A;
    u16 unk7C;
    u16 unk7E;
    u16 unk80;
    u16 unk82;
    u16 unk84;
    u16 unk86;
    s16 unk88;
    s16 unk8A;
    u8 unk8C;
    u8 unk8D;
    s8 unk8E;
    u8 unk8F;
    u8 unk90;
    u8 unk91;
    u8 unk92;
    u8 unk93;
    u8 unk94;
    u8 unk95;
    s16 unk96;
    s16 unk98;
    s16 unk9A;
    u16 unk9C;
    u16 unk9E;
    u16 unkA0;
    u8 unkA2;
    u8 unkA3;
    u16 unkA4;
    s16 unkA6;
    void *unkA8;
} Obj43430;

typedef struct {
    u8 unk00[0x34];
    u16 unk34;
    u16 unk36;
    u16 unk38;
    u16 unk3A;
    u16 unk3C;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    u16 unk44;
    u16 unk46;
    u16 unk48;
    u16 unk4A;
    u16 unk4C;
    u16 unk4E;
    u8 unk50;
    u8 unk51;
    u8 unk52;
    u8 unk53;
    u8 unk54;
    u8 unk55;
    u8 unk56;
    u8 unk57;
    u16 unk58;
    u8 unk5A;
    u8 unk5B;
    u16 unk5C;
    u8 unk5E;
    u8 unk5F;
} Def43430;

extern void func_8003C14C(Obj43430 *p, Def43430 *src);
extern s32 func_80017B88(void);
extern void *func_80017EBC(s32 size, s32 tag);
extern void func_800188D0(s32 x, s32 y, s32 z, s32 *out);
extern void func_80043250(Obj43430 *p);

u8 func_80043430(Obj43430 *p, Def43430 *src)
{
    s32 matrix[9];
    s32 i;
    s32 limit;
    u16 value;

    func_8003C14C(p, src);
    limit = 0x2BC;

    if ((s16)src->unk36 == -1) {
        src->unk36 = src->unk34;
    }
    if ((s16)src->unk36 > limit) {
        src->unk36 = limit;
    }
    if ((s16)src->unk36 <= 0) {
        return 0;
    }
    if ((s16)src->unk36 < (s16)src->unk34) {
        src->unk34 = src->unk36;
    }
    if (func_80017B88() == 1) {
        if ((s16)src->unk34 >= 0xB) {
            src->unk34 = (s16)src->unk34 >> 1;
            src->unk36 = (s16)src->unk36 >> 1;
        }
    }

    p->unk8A = src->unk36;
    p->unk88 = src->unk34;
    p->unk6C = 0;
    p->unk68 = 0;
    p->unk7C = src->unk44;
    p->unk72 = src->unk3C;
    p->unk8F = src->unk51;
    p->unk90 = src->unk52;
    p->unk91 = src->unk53;
    p->unk92 = src->unk54;
    p->unk93 = src->unk55;
    p->unk94 = src->unk56;
    p->unk80 = src->unk38;
    p->unk82 = src->unk3A;
    p->unk7E = src->unk46;
    p->unk9C = src->unk48;
    p->unk9E = src->unk4A;
    p->unk84 = src->unk4C;
    p->unk86 = src->unk4E;
    p->unkA2 = src->unk5E & 0xF;
    p->unkA3 = src->unk5F & 0xF;

    value = src->unk58;
    p->unkA6 = value;
    if ((s16)value < 0) {
        p->unkA6 = -value;
    }

    p->unk8E = ((s8)src->unk50) >> 1;
    if (p->unk8E >= 8) {
        p->unk8E = 7;
    } else if (p->unk8E < -7) {
        p->unk8E = -7;
    } else if (p->unk8E == 0) {
        p->unk8E = 1;
    }

    p->unk8C = src->unk5A;
    p->unk8D = src->unk5B;
    p->unkA0 = src->unk5C;

    func_800188D0(src->unk3E, -src->unk40, 0, matrix);
    p->unk76 = ((matrix[6] >> 4) * src->unk42) >> 12;
    p->unk78 = ((matrix[7] >> 4) * src->unk42) >> 12;
    p->unk7A = ((matrix[8] >> 4) * src->unk42) >> 12;

    p->unk6C = func_80017EBC(p->unk8A * 0x1C, 1);
    if (p->unk6C == 0) {
        return 0;
    }

    for (i = 0; i < p->unk8A - 1; i++) {
        ((Node43430 *)p->unk6C)[i].next = &((Node43430 *)p->unk6C)[i + 1];
    }
    ((Node43430 *)p->unk6C)[i].next = 0;

    p->unk64 = p->unk6C;
    p->unk68 = 0;
    p->unkA4 = 0;
    p->unk74 = 0;
    p->unk70 = 0;
    p->unk72 = (-p->unk72 << 12) / 100;
    func_80043250(p);

    if (p->unk88 < p->unk8A) {
        p->unkA8 = func_80017EBC(p->unk88 * 0x1C, 1);
    } else {
        p->unkA8 = 0;
    }
    return 1;
}
