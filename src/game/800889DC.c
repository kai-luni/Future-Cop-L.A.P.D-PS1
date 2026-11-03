#include "game/types.h"

struct CdlLOC {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

struct DiscThing800889DC {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    struct CdlLOC loc0C;
    u8 pad10[0x14];
    struct CdlLOC loc24;
    u8 pad28[0x8];
    s32 unk30;
};

struct State800889DC {
    s32 unk00;
    s32 unk04;
    s32 unk08[0x100];
    u32 unk408;
    s32 unk40C;
    s32 unk410;
    s32 unk414;
    s32 unk418;
    s32 unk41C;
    s32 unk420;
    u32 unk424;
    s32 unk428;
    s32 unk42C;
    volatile struct DiscThing800889DC *unk430;
    u8 pad434[0xC];
    u32 unk440;
    u32 unk444;
    u32 unk448;
    s32 unk44C;
    s32 unk450;
    s32 unk454;
    u8 unk458[0x46];
};

extern volatile struct State800889DC *D_8009F970;
extern s32 D_8009F508;
extern volatile u32 *D_8009F50C;
extern u32 D_8009F980;

extern s32 func_800886C8(s32, s32);
extern void func_8008888C(s32, s32, s32);
extern s32 func_8008E2F8(s32, s32);
extern s32 func_8008E598(void *, s32);
extern s32 func_8008E5FC(s32);
extern struct CdlLOC *func_8008E61C(s32, struct CdlLOC *);
extern s32 func_8008E720(struct CdlLOC *);
extern void func_800889A8(s32, s32, s32);

struct CdlLOC *func_800889DC(struct CdlLOC *loc, struct CdlLOC *out, s32 offset)
{
    return func_8008E61C(func_8008E720(loc) + offset, out);
}

struct Loc80088A1C {
    struct CdlLOC loc;
    u8 pad[0x1C];
};


void func_80088A1C(u8 mode)
{
    struct Loc80088A1C tmp;
    s32 sector;
    register volatile struct State800889DC *p __asm__("$8");
    u32 count;
    volatile struct DiscThing800889DC *disc1;
    volatile struct DiscThing800889DC *disc2;
    volatile struct DiscThing800889DC *disc4;
    volatile struct DiscThing800889DC *disc5;
    volatile struct DiscThing800889DC *disc6;
    struct DiscThing800889DC *disc7;
    s32 a4;
    s32 b4;
    s32 r0;

    if (mode != 1) {
        D_8009F970->unk424++;
        return;
    }

    if (D_8009F970->unk424 >= 2) {
        return;
    }
    D_8009F970->unk424 = 0;
    if (D_8009F970->unk414 != 2) {
        return;
    }
    if (D_8009F970->unk00 == 0) {
        return;
    }

    if (D_8009F508 != 0) {
        count = 0;
        while (*D_8009F50C & 0x1000000) {
            count++;
            if (count == 0x10001) goto done;
        }
        if (count != 0) count--;
done:
        ;
    }

    if (!(D_8009F970->unk04 & 3)) {
        func_8008E598(&tmp.loc, 3);
        func_8008E5FC(0);
        sector = func_8008E720(&tmp.loc);

        disc1 = D_8009F970->unk430;
        func_800889DC(&disc1->loc0C, &disc1->loc24, disc1->unk08);
        //(&disc->loc0C, &disc->loc24, disc->unk08);

        if (func_8008E720(&D_8009F970->unk430->loc24) != sector) {
            func_8008E598((void *)D_8009F970->unk04, 0x200);
            func_8008E5FC(0);
            func_8008E598(D_8009F970->unk458, 0x46);
            func_8008E5FC(0);
            disc2 = D_8009F970->unk430;
            func_800889DC(&disc2->loc0C, &disc2->loc24, disc2->unk08);
            disc4 = D_8009F970->unk430;
            D_8009F970->unk428++;
            D_8009F970->unk418 = 2;
            D_8009F970->unk414 = 1;
            func_8008888C(0x15, (s32)&disc4->loc24, 0);
            a4 = D_8009F970->unk04;
            disc5 = D_8009F970->unk430;
            b4 = D_8009F970->unk00;
            func_800889A8(a4, b4, (s32)&disc5->loc24);
        } else {
            func_8008E598((void *)D_8009F970->unk04, 0x200);
            func_8008E5FC(0);
            func_8008E598(D_8009F970->unk458, 0x46);
            func_8008E5FC(0);

            p = D_8009F970;
            p->unk04 += 0x800;
            r0 = p->unk00;
            disc6 = p->unk430;
            p->unk00 = r0 - 1;
            disc6->unk08++;
            p->unk448++;
            if (p->unk00 == 0) {
                register u32 a0v __asm__("$4");
                register u32 dividend __asm__("$2");
                register s32 u08 __asm__("$3");
                register s32 rem __asm__("$6");
                s32 temp;
                s32 c;
                s32 d;
                a0v = (u32)p->unk430;
                sector = p->unk00;
                u08 = ((volatile struct DiscThing800889DC *)a0v)->unk08;
                dividend = p->unk448 + sector;
                rem = dividend % D_8009F980;
                temp = ((volatile struct DiscThing800889DC *)a0v)->unk00 - (u08 + sector);
                a0v = p->unk440 + D_8009F980 - dividend;
                c = func_800886C8(a0v, D_8009F980 - rem);
                d = func_800886C8(c, temp);
                if (d != 0) {
                    disc7 = (struct DiscThing800889DC *)D_8009F970->unk430;
                    if (D_8009F970->unk04 == disc7->unk30 + (D_8009F980 << 11)) {
                        D_8009F970->unk04 = disc7->unk30;
                    }
                    D_8009F970->unk00 = d;
                } else {
                    D_8009F970->unk00 = 0;
                    D_8009F970->unk414 = 0;
                }
            }
        }
    } else {
        D_8009F970->unk414 = 0;
    }
}

void D_80088D74(u8 mode)
{
    s32 a;
    s32 b;

    if (mode != 2) {
        D_8009F970->unk424++;
        return;
    }

    if (D_8009F970->unk424 >= 2) {
        return;
    }
    D_8009F970->unk424 = 0;
    if (D_8009F970->unk41C == 0) {
        return;
    }
    if (D_8009F970->unk410 == 0) {
        return;
    }
    if (D_8009F970->unk418 != 0) {
        D_8009F970->unk418--;
        if (D_8009F970->unk418 == 0) {
            {
                s32 x = D_8009F970->unk450;
                s32 y = D_8009F970->unk454;
                D_8009F970->unk04 = x;
                D_8009F970->unk00 = y;
            }
            D_8009F970->unk414 = mode;
        }
    }
    D_8009F970->unk410--;
    D_8009F970->unk408 += 3;
    D_8009F970->unk408 %= 48;
    if (D_8009F970->unk410 == 0) {
        return;
    }
    {
        s32 t = D_8009F970->unk08[D_8009F970->unk408];
        s32 b = D_8009F970->unk08[D_8009F970->unk408 + 1];
        s32 a = t;
        (void)D_8009F970->unk08[D_8009F970->unk408 + 2];
        while (func_8008E2F8(a & 0xFF, b) == 0) {
            ;
        }
    }
}
