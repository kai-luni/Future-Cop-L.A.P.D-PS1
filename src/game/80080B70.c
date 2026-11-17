// Categories: Resources > Save Data | Platform > File Transfer | Game Engine > Front End
#include "game/types.h"

typedef struct {
    u8 pad0[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad21[3];
} Entry;

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 *D_8009FA94;
extern u8 D_800A6FE0[];
extern Entry D_8009B4E4[];
extern s8 D_8009B72C[];
extern u8 D_800120A4[];
extern u8 D_8009F3C0;
extern u8 D_8009F3C1;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F3E6;
extern u8 D_8009F3E7;
extern u8 D_8009F458;
extern u8 D_8009F3F1;
extern u8 D_8009FAC4;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;

extern u8 *func_80089914(u16 id);
extern void func_800822CC(u8 *arg0);
extern u8 *func_8001E318(u8 *destination, u8 *source, u32 count);
extern void func_80080A5C(u8 *arg0);
extern void func_8008CCB0(u8 *arg0, u8 *arg1);
extern void func_8007F6E0(void);
extern s32 func_80020FDC(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3);

typedef struct { u8 b[0x70]; } Rec;
typedef struct { u8 pad0[0x424]; Rec recs[10]; u8 pad884[0xA0]; u8 idx; } Big;
#define S(off) (((Big *)gSaveProfileImage)->recs[((Big *)gSaveProfileImage)->idx].b[(off) - 0x424])
#define REC (((Rec *)((Big *)gSaveProfileImage)->recs)[((Big *)gSaveProfileImage)->idx].b - 0x424)

/**
 * @brief Materialises the 0x678-byte front-end profile image in the D_8009F92C block
 *        from the current settings, starts its file transfer (three polls) and falls
 *        back to the default screen state when it cannot be started.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Image build:
 * The block layout used here is the one func_8008125C documents: a 0x200-byte live
 * header, screen-state bytes at +0x200..+0x204, a 0x10-byte payload with two
 * checksum words at +0x40C (checksums at +0x41C and +0x420), and ten 0x70-byte
 * records from +0x424 (0x68-byte payload plus two checksum words at +0x68 and +0x6C);
 * the file image itself is +0x20C..+0x884. func_8001E318 copies the live 0x200-byte
 * header over the image header at +0x20C, and func_800822CC decodes the text
 * func_80089914(0x36C4) returns into the block's +0x04 area as glyph half-words.
 * The first image word at +0x40C is set to 1, then func_80080A5C fills the option
 * block at +0x40C and the record's own audio block at +0x474 with the four mixer
 * bytes (+0x04..+0x07), the two device bytes (+0x08 from D_8009FA98, +0x09 from
 * D_8009FA99[0]) and two volumes (+0x0A/+0x0B from func_8001EB2C).
 *
 * Record fields:
 * Only the record selected by the block's index byte is written, addressed with
 * absolute-equivalent offsets: two 9-byte names are strcpy'd from D_800A6FE0 and
 * D_800A6FE0 + 9 into +0x440 and +0x44C, the 0x1D-byte scratch name at +0x925 goes
 * to +0x428, +0x458 takes D_8009F458 (also mirrored into D_8009F3F1), +0x459..+0x463
 * and +0x468/+0x470 take the mission words D_8009FA94 +0x8C, +0x90, +0x94, +0x28,
 * +0x34, +0x2C, +0x38, +0x30, +0x3C, +0x08, +0x98, +0x0C and +0x5C, +0x469..+0x46D
 * take D_8009F3C0, D_8009F3E6, D_8009F3E7, D_8009F3C1 and D_8009F3C3, +0x46F takes
 * D_8009F3C4, +0x464..+0x467 take the port bytes D_8009B4E4[D_8009B72C[i]].unk1F for
 * i = 0..3 and +0x46E takes port 4. func_8007F6E0 then recomputes both checksums and
 * the same five port entries get unk20 = unk1F.
 *
 * Transfer loop:
 * +0x922 is armed with 3, +0x201 with 2, +0x202 with 1, D_8009FAC4 with 1, and
 * D_8009F8BD/D_8009F8BE are cleared. The loop calls func_80020FDC(D_800120A4,
 * pointer +0x20C, 0x678, 0): a return of 0x2000 means the transfer was queued
 * (func_8001F180 found the transfer unit idle and unk9A4, read back by
 * func_8001F37C, stayed zero), a return of 0 means it was refused because the unit
 * was still busy. On 0x2000 the loop breaks; otherwise it decrements +0x922 and
 * retries, and when the counter runs out it writes +0x200 = 2, +0x201 = 0,
 * +0x204 = 2, +0x202 = 3 and re-arms D_8009F8BD and D_8009F8BE.
 *
 * @par Likely role
 * The "commit this profile slot" step of the front-end profile list: the record is
 * assembled from the live options, its checksums are refreshed, and the resulting
 * image is handed to the file layer whose slot manager (func_80020C90 for an
 * existing file, func_80020E68 for a new one of that size) matches names.
 *
 * @par Unknown
 * Whether the 0x678-byte transfer reads or writes - the image is seeded before the
 * call, which fits both a save and a load whose defaults survive a missing file -
 * and why the movie step func_8007FB04 also calls this function from its stream
 * retry branch.
 *
 * @par Callers
 * func_8007FB04, the status-change branch of the movie step, and func_8008125C, the
 * profile list widget, after a name edit is decoded back into the record.
 */
void func_80080B70(void)
{
    u8 *r;
    u32 i;

    r = func_80089914(0x36C4);
    if (r != 0) {
        func_800822CC(r);
    }
    func_8001E318(gSaveProfileImage + 0x20C, gSaveProfileImage, 0x200);
    *(s32 *)(gSaveProfileImage + 0x40C) = 1;
    func_80080A5C(gSaveProfileImage + 0x40C);
    func_80080A5C(REC + 0x474);
    func_8008CCB0(REC + 0x440, D_800A6FE0);
    func_8008CCB0(REC + 0x44C, D_800A6FE0 + 9);
    S(0x459) = D_8009FA94[0x8C];
    S(0x45A) = D_8009FA94[0x90];
    S(0x45B) = D_8009FA94[0x94];
    S(0x45C) = D_8009FA94[0x28];
    S(0x45D) = D_8009FA94[0x34];
    S(0x45E) = D_8009FA94[0x2C];
    S(0x45F) = D_8009FA94[0x38];
    S(0x460) = D_8009FA94[0x30];
    S(0x461) = D_8009FA94[0x3C];
    S(0x462) = D_8009FA94[0x8];
    S(0x463) = D_8009FA94[0x98];
    S(0x470) = D_8009FA94[0xC];
    S(0x468) = D_8009FA94[0x5C];
    S(0x469) = D_8009F3C0;
    S(0x46A) = D_8009F3E6;
    S(0x46B) = D_8009F3E7;
    S(0x46C) = D_8009F3C1;
    S(0x46D) = D_8009F3C3;
    S(0x46F) = D_8009F3C4;
    S(0x46E) = D_8009B4E4[D_8009B72C[4]].unk1F;
    for (i = 0; i < 4; i++) {
        S(0x464 + i) = D_8009B4E4[D_8009B72C[i]].unk1F;
    }
    S(0x458) = D_8009F458;
    D_8009F3F1 = D_8009F458;
    func_8008CCB0(REC + 0x428, gSaveProfileImage + 0x925);
    func_8007F6E0();
    gSaveProfileImage[0x922] = 3;
    gSaveProfileImage[0x201] = 2;
    {
        register u8 *p __asm__("$2") = gSaveProfileImage;
        __asm__("" : "+r"(p));
        p[0x202] = 1;
        D_8009FAC4 = 1;
    }
    D_8009F8BD = 0;
    D_8009F8BE = 0;
    D_8009B4E4[D_8009B72C[0]].unk20 = D_8009B4E4[D_8009B72C[0]].unk1F;
    D_8009B4E4[D_8009B72C[1]].unk20 = D_8009B4E4[D_8009B72C[1]].unk1F;
    D_8009B4E4[D_8009B72C[2]].unk20 = D_8009B4E4[D_8009B72C[2]].unk1F;
    D_8009B4E4[D_8009B72C[3]].unk20 = D_8009B4E4[D_8009B72C[3]].unk1F;
    D_8009B4E4[D_8009B72C[4]].unk20 = D_8009B4E4[D_8009B72C[4]].unk1F;
    for (;;) {
        if (func_80020FDC(D_800120A4, gSaveProfileImage + 0x20C, 0x678, 0) != 0) {
            break;
        }
        if (*(s8 *)(gSaveProfileImage + 0x922) != 0) {
            gSaveProfileImage[0x922] = gSaveProfileImage[0x922] - 1;
            continue;
        }
        gSaveProfileImage[0x200] = 2;
        gSaveProfileImage[0x201] = 0;
        gSaveProfileImage[0x204] = 2;
        gSaveProfileImage[0x202] = 3;
        D_8009F8BD = 1;
        D_8009F8BE = 1;
        break;
    }
}
