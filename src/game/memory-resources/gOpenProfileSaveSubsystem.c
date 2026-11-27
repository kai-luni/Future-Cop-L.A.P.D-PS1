// Categories: Resources > Save Data | Platform > Memory Card
#include "game/types.h"

struct Unk8007F9CC {
    u8 pad0[0x200];
    u8 unk200;
    u8 pad201[0x7];
    u32 unk208;
    u8 pad20C[0x6F8];
    void *volatile unk904;
    u8 pad908[0x3D];
    u8 unk945;
    u8 pad946[0x2];
};

extern struct Unk8007F9CC *gSaveProfileImage __asm__("D_8009F92C");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern int gGetMcStateSize(void) __asm__("func_80020610");
extern void gInitMcState(s32 arg0, void *arg1) __asm__("func_80020618");

/**
 * @brief Opens the profile save subsystem.
 *
 * @return Nothing; both blocks are stored in globals.
 *
 * @par Confirmed flow
 * 1. p = func_80017EBC(0x948, 2); D_8009F92C = p; the block is zeroed with
 *    func_8001E4C0(p, 0, 0x948).
 * 2. unk208 = 3 and unk945 = 2 are pre-set (result code and page defaults).
 * 3. A block of func_80020610() (0x1BB8) bytes is allocated into +0x904, +0x200
 *    is set to 1 and func_80020618(0, block) installs the memory-card manager
 *    inside it.
 *
 * @par Confirmed role
 * The 0x948-byte block is the profile context: live 0x200-byte header at +0, the
 * state bytes +0x200..+0x208, the 0x678-byte card image at +0x20C, the option
 * block at +0x40C, ten 0x70-byte records at +0x424 and the page/retry bytes at
 * +0x921/+0x922. The second block is the memory-card manager, not an MDEC/XA
 * decoder (see func_800206D0).
 *
 * @par Callers
 * func_80076A2C (boot) and func_800780DC (module start).
 */
void gOpenProfileSaveSubsystem(void) __asm__("func_8007F9CC");

void gOpenProfileSaveSubsystem(void)
{
    void *p;
    void *q;
    struct Unk8007F9CC *s;

    p = gAllocMemory(0x948, 2);
    gSaveProfileImage = p;
    gFillMemory(p, 0, 0x948);
    gSaveProfileImage->unk208 = 3;
    gSaveProfileImage->unk945 = 2;
    q = gAllocMemory(gGetMcStateSize(), 2);
    s = gSaveProfileImage;
    s->unk200 = 1;
    s->unk904 = q;
    gInitMcState(0, gSaveProfileImage->unk904);
}
