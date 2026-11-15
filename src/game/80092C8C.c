// Categories: Platform > GPU
#include "game/types.h"

typedef struct {
    u8 unk00[0x3C];
    void (*unk3C)(s32);
} Unk8009DDE8;

extern u8 D_8009DDF2;
extern void (*D_8009DDEC)(void *arg0, s32 arg1);
extern Unk8009DDE8 *D_8009DDE8;
extern u8 D_80012A44[];

/**
 * @brief GPU synchronisation (DrawSync-style): waits for or flushes the
 *        graphics engine.
 *
 * @param arg0 Mode forwarded to the GPU library, 0 = "wait for completion".
 *
 * @par Confirmed mechanics
 * If the debug level D_8009DDF2 is >= 2 the hook D_8009DDEC is called with
 * D_80012A44; then D_8009DDE8->unk3C(arg0) is the library entry point that
 * performs the sync.
 *
 * @par Globals
 * D_8009DDF2 (debug level), D_8009DDEC (debug hook), D_8009DDE8 (GPU library
 * state, function pointer at +0x3C).
 *
 * @par Callers
 * Every frame: func_8001B4A8, func_800259D8, func_8003FAD4, func_80025B7C and
 * the video layer all bracket their GPU work with it.
 */
void func_80092C8C(s32 arg0)
{
    if (D_8009DDF2 >= 2) {
        D_8009DDEC(D_80012A44, arg0);
    }

    D_8009DDE8->unk3C(arg0);
}
