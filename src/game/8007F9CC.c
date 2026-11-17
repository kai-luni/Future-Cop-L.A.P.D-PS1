// Categories: Resources > Movie Playback | Platform > MDEC
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

extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern int func_80020610(void);
extern void func_80020618(s32 arg0, void *arg1);

/**
 * @brief Opens the movie player: allocates its 0x948-byte context and the
 *        decoder work buffer.
 *
 * @return Nothing; both blocks are stored in globals.
 *
 * @par Confirmed flow
 * 1. p = func_80017EBC(0x948, 2); D_8009F92C = p; the block is zeroed with
 *    func_8001E4C0(p, 0, 0x948).
 * 2. unk208 = 3 and unk945 = 2 are pre-set (state and sub-state defaults).
 * 3. A second block of func_80020610() bytes is allocated and stored in
 *    unk904; unk200 is set to 1 and func_80020618(0, unk904) initialises the
 *    decoder structure inside it.
 *
 * @par Likely role
 * The context layout (0x200-byte header, state byte at 0x200, work pointer at
 * 0x904, flags at 0x945) is the movie player's own state block; the second
 * allocation is sized by the decoder module (func_80020610), which owns the
 * MDEC/XA data structures.
 *
 * @par Callers
 * func_80076A2C (boot) and func_800780DC (module start).
 */
void func_8007F9CC(void)
{
    void *p;
    void *q;
    struct Unk8007F9CC *s;

    p = func_80017EBC(0x948, 2);
    gSaveProfileImage = p;
    func_8001E4C0(p, 0, 0x948);
    gSaveProfileImage->unk208 = 3;
    gSaveProfileImage->unk945 = 2;
    q = func_80017EBC(func_80020610(), 2);
    s = gSaveProfileImage;
    s->unk200 = 1;
    s->unk904 = q;
    func_80020618(0, gSaveProfileImage->unk904);
}
