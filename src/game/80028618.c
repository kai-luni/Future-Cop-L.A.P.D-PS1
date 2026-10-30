#include "game/types.h"

extern volatile s32 D_8009FA60;
extern s32 D_8009F9FC;
extern s32 D_8009FA50;
extern s32 D_8009FAA4;
extern s32 D_8009FA30;
extern s32 D_8009FA00;
extern s32 D_8009FA0C;
extern u8 D_8009F690;
extern u8 D_8009F691;
extern u8 D_8009F692;
extern u8 D_8009F693;
extern u8 D_8009FA3C;
extern void *D_8009F9E4;

/* Incomplete array types keep these out of small data on the MIPS backend,
   which is why the oracle addresses them with lui/lw instead of ($gp). */
extern u8 *D_8009F9A0[];
extern u8 *D_8009FA58[];

extern void func_8001BFD8(u8 value);
extern void func_8002703C(void);
extern void func_800271A4(void);
extern void func_80027510(void);
extern void func_80028164(void);
extern void func_80027D9C(void);
extern void func_80027D70(void);
extern void func_8001B4A8(s32 arg);
extern void func_800451EC(s32 value);
extern void func_800462D0(void);
extern void func_8008352C(s32 arg);
extern void func_80017B60(s32 id);
extern void func_80045884(s32 *position, s32 arg1, s32 angle, s32 index);

void func_80028618(void)
{
    register s32 saved __asm__("$3");
    register u32 b __asm__("$2");
    register s32 work __asm__("$16");
    register u8 **cursor __asm__("$17");

    D_8009FA60 = 0;
    saved = D_8009FA60;
    work = 8;
    D_8009F9FC = work;
    b = *(u8 *)&D_8009F9FC;
    D_8009FA50 = 0;
    D_8009F693 = 0;
    D_8009FAA4 = saved;
    D_8009FA30 = saved - 8;
    D_8009F690 = b;
    D_8009F691 = b;
    D_8009F692 = b;

    func_8001BFD8(0);

    if (D_8009FA00 != 4 && D_8009FA00 != 0xD && D_8009FA00 != work) {
        goto finished;
    }

frame:
    {
        D_8009FA50 = D_8009FA50 + 1;
        func_8002703C();
        if (D_8009FA0C == 1) {
            func_800271A4();
        } else {
            func_80027510();
        }
        {
            register s32 frame_id __asm__("$4");
            frame_id = D_8009F9FC;
            cursor = D_8009FA58;
            work = 0;
            func_8008352C(frame_id);
        }

        do {
            register u8 *position __asm__("$4");
            register u8 *camera __asm__("$5");
            register s32 angle __asm__("$6");

            if (*cursor == 0) {
                break;
            }
            func_80017B60((s32)*(s8 *)(*(u8 **)(*cursor + 0xAC) + 0xAC));
            position = D_8009F9A0[0];
            if (position != 0) {
                position += 0x50;
                camera = (u8 *)D_8009F9E4;
                angle = *(s32 *)(camera + 0x20);
                camera += 0x7C;
                func_80045884((s32 *)position, (s32)camera, angle, work);
            } else {
                position = *cursor + 0x50;
                camera = (u8 *)D_8009F9E4;
                angle = *(s32 *)(camera + 0x20);
                camera += 0x7C;
                func_80045884((s32 *)position, (s32)camera, angle, work);
            }
            work++;
            cursor++;
        } while (work < 2);

        func_800451EC(work);
        work = 0xD;
        func_800462D0();

        if (D_8009FA00 == work) {
            func_80028164();
            func_8001B4A8(1);
        }

        func_80027D70();

        if (D_8009FA00 == 4 || D_8009FA00 == work) {
            func_80027D9C();
            if (D_8009FA00 != work) {
                D_8009FA3C = 1;
            }
        } else {
            D_8009FA3C = 1;
        }
    }

    if (D_8009FA00 == 4 || D_8009FA00 == 0xD || D_8009FA00 == 8) {
        goto frame;
    }
finished:
    func_8001BFD8(1);
}
