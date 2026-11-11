#include "game/types.h"

struct Mat12 {
    s32 w[12];
};

struct Blk3 {
    s32 w[3];
};

struct Ptr4 {
    s32 pad0;
    u8 *unk4;
};

struct S3 {
    u8 pad00[0x14];
    u8 *unk14;
    s32 unk18;
    struct Ptr4 *unk1C;
    struct Ptr4 *unk20;
};

struct Arg0 {
    struct S3 *unk00;
    u8 pad04[0x44];
    s16 unk48;
};

extern struct Mat12 D_8009AA38;
extern struct Mat12 D_800A0300;
extern struct Blk3 D_800A02E0;
extern struct Blk3 D_800A02F0;
extern u8 D_8009FAE0[0x100];
extern u8 *D_8009F570;
extern u8 *D_8009F574;
extern u8 *D_8009F578;
extern u8 *D_8009F57C;

extern void func_800178DC(void *object, s32 arg1);
extern void func_80013ECC(u32 *arg0);
extern void func_80013CB8(s32 a0, u8 *a1, s32 a2, s32 a3);
extern void func_80014164(u8 *a0, u8 *a1, void *a2, void *a3);
extern void func_80010750(void *a0);
extern void func_8001010C(u8 *a0, u8 *a1, s32 a2);
extern void func_8001047C(u8 *a0, u8 *a1, u8 *a2, s32 a3);

void func_8001428C(struct Arg0 *arg0, u8 *arg1)
{
    register struct S3 *s3 __asm__("$19");
    register struct Mat12 *dest __asm__("$20");
    register u8 *fae0 __asm__("$22");
    register u32 *callarg __asm__("$4");
    u8 *p;
    register u8 *tblA __asm__("$23");
    u8 *tblB;
    s32 buf1[12];
    s32 buf2[12];
    u8 idx;
    register struct Arg0 *reload __asm__("$24");
    u8 *object;
    s32 count;
    register u8 *callp __asm__("$5");

    object = (u8 *)arg0 + 4;
    __asm__("" : "+m"(arg0));
    reload = arg0;
    s3 = reload->unk00;
    func_800178DC(object, (s32)buf2);
    /* Ordered matrix/global setup; the call and render loop remain C. */
    __asm__ __volatile__(
        "lui\t$17,%%hi(D_800A02F0)\n\t"
        "lui\t$16,%%hi(D_800A0300)\n\t"
        "addiu\t$14,$16,%%lo(D_800A0300)\n\t"
        "lui\t$15,%%hi(D_8009AA38)\n\t"
        "addiu\t$2,$15,%%lo(D_8009AA38)\n\t"
        "lw\t$3,4($2)\n\t"
        "lw\t$5,8($2)\n\t"
        "lw\t$6,12($2)\n\t"
        "lw\t$7,16($2)\n\t"
        "lw\t$8,20($2)\n\t"
        "lw\t$9,24($2)\n\t"
        "lw\t$10,28($2)\n\t"
        "lw\t$11,32($2)\n\t"
        "lw\t$12,36($2)\n\t"
        "lw\t$13,40($2)\n\t"
        "lw\t$2,44($2)\n\t"
        "lw\t%4,0x368($28)\n\t"
        "lw\t$23,0x364($28)\n\t"
        "addu\t$20,$14,$0\n\t"
        "sw\t$2,44($20)\n\t"
        "lui\t$2,%%hi(D_8009FAE0)\n\t"
        "addiu\t$2,$2,%%lo(D_8009FAE0)\n\t"
        "addu\t$22,$2,$0\n\t"
        "sw\t$3,4($20)\n\t"
        "sw\t$5,8($20)\n\t"
        "sw\t$6,12($20)\n\t"
        "sw\t$7,16($20)\n\t"
        "sw\t$8,20($20)\n\t"
        "sw\t$9,24($20)\n\t"
        "sw\t$10,28($20)\n\t"
        "sw\t$11,32($20)\n\t"
        "sw\t$12,36($20)\n\t"
        "sw\t$13,40($20)\n\t"
        "lw\t%2,20($19)\n\t"
        "lw\t$3,%%lo(D_8009AA38)($15)\n\t"
        "lw\t$2,28($19)\n\t"
        "addiu\t$4,$17,%%lo(D_800A02F0)\n\t"
        "sw\t$22,0x35C($28)\n\t"
        "sw\t$3,%%lo(D_800A0300)($16)\n\t"
        "lui\t$3,%%hi(D_800A02E0)\n\t"
        "lw\t$2,4($2)\n\t"
        "addiu\t$8,$3,%%lo(D_800A02E0)\n\t"
        "addiu\t$7,$17,%%lo(D_800A02F0)\n\t"
        "lw\t$5,0($8)\n\t"
        "lw\t$6,4($8)\n\t"
        "sw\t$5,0($7)\n\t"
        "sw\t$6,4($7)\n\t"
        "lw\t$5,8($8)\n\t"
        "nop\n\t"
        "sw\t$5,8($7)\n\t"
        "addiu\t$2,$2,4\n\t"
        "sw\t$2,0x360($28)\n\t"
        : "=&r"(dest), "=&r"(fae0), "=&r"(p), "=&r"(tblA), "=&r"(tblB), "=&r"(callarg)
        : "r"(s3)
        : "$2", "$3", "$5", "$6", "$7", "$8", "$9", "$10", "$11",
          "$12", "$13", "$14", "$15", "$16", "$17", "memory"
    );
    func_80013ECC(callarg);

    for (;;) {
        idx = p[0];
        callp = p;
        __asm__("" : : "r"(callp) : "$6");
        count = s3->unk18;
        __asm__("" : : "r"(count) : "$24");
        reload = arg0;
        func_80013CB8(idx, callp, count, reload->unk48);
        func_80014164(arg1, (u8 *)(idx * 0x30 + (s32)dest), buf1, buf2);
        func_80010750(buf1);
        func_8001010C(s3->unk20->unk4 + (p[1] * 8 + 4), (u8 *)(p[1] * 8 + (s32)fae0),
                      p[2]);
        func_8001047C(s3->unk1C->unk4 + (p[3] * 8 + 4), tblA + p[3] * 8,
                      tblB + p[3], p[4]);
        if (p[7] & 0x80) {
            break;
        }
        p += 0x14;
        arg1 += 0x14;
    }
}
