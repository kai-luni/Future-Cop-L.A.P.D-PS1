#include "game/types.h"

struct Unk8003F854 {
    struct Unk8003F854 *next;
    u8 unk04[0x14];
    void *unk18;
    u8 unk1C[0x08];
    void (*unk24)(s32, struct Unk8003F854 *, s32, s32);
};

extern struct Unk8003F854 *D_800A1070[7];
extern struct Unk8003F854 *D_8009F314;
extern struct Unk8003F854 *D_8009F30C;
extern void *D_800A7040[0x200];
extern void *D_800A0C70[0x100];

extern void func_800181D4(void *block);
extern void func_8003D0AC(void);
extern void func_8006EDF0(void);
extern void func_8005E22C(void);

void func_8003F854(s32 arg0)
{
    register s32 mode __asm__("$20");

    mode = arg0;
    {
        register s32 i __asm__("$17");
        struct Unk8003F854 **head;
        register struct Unk8003F854 *p __asm__("$5");
        register struct Unk8003F854 *next __asm__("$16");
        register s32 zero __asm__("$6");

        i = 0;
        head = D_800A1070;
        do {
            p = *head;
            while (p != 0) {
                next = p->next;
                zero = 0;
                p->unk24(10, p, zero, zero);
                p = next;
            }
            i++;
            head++;
        } while (i < 7);

        {
            register u32 clear __asm__("$2");

            i = 6;
            clear = (u32)D_800A1070;
            clear += 0x18;
            do {
                *(struct Unk8003F854 **)clear = 0;
                i--;
                clear -= 4;
            } while (i >= 0);
        }

        p = D_8009F314;
        while (p != 0) {
            next = p->next;
            zero = 0;
            p->unk24(10, p, zero, zero);
            p = next;
        }
        D_8009F314 = 0;
    }

    {
        register s32 bank __asm__("$18");
        register s32 both __asm__("$21");
        void **row;

        bank = 0;
        both = 2;
        row = D_800A7040;
        do {
            if (mode == both || mode == bank) {
                register s32 i __asm__("$17");
                register void **slot __asm__("$16");

                i = 0;
                slot = row;
                do {
                    if (*slot != 0) {
                        func_800181D4(*slot);
                    }
                    *slot = 0;
                    i++;
                    slot++;
                } while (i < 0x100);
            }
            bank++;
            row += 0x100;
        } while (bank < 2);
    }

    {
        register s32 i __asm__("$17");
        void **slot;

        i = 0;
        slot = D_800A0C70;
        do {
            if (*slot != 0) {
                func_800181D4(*slot);
            }
            *slot = 0;
            i++;
            slot++;
        } while (i < 0x100);
    }

    {
        register struct Unk8003F854 *p __asm__("$16");
        register struct Unk8003F854 *next __asm__("$17");

        p = D_8009F30C;
        while (p != 0) {
            next = p->next;
            if (p->unk18 != 0) {
                func_800181D4(p->unk18);
            }
            func_800181D4(p);
            p = next;
        }
        D_8009F30C = 0;
    }

    func_8003D0AC();
    func_8006EDF0();
    func_8005E22C();
}
