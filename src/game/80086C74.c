// Categories: Platform > Audio
#include "game/types.h"

struct State86 {
    char pad[0x7B20];
    s32 active;
    s32 value;
    s32 count;
    char pad2[0x38];
    void *first;
    void *second;
};
struct Setup86 {
    s32 type;
    s16 a;
    s16 b;
    char pad[0x20];
};
struct Msg86 {
    void *object;
    s32 kind;
    s16 a;
    s16 b;
    char pad0[8];
    s16 c;
    char pad1[6];
    s32 value;
    char pad2[4];
    s32 one;
    s32 one2;
    s32 three;
    s16 d;
    s16 e;
    s16 f;
    s16 g;
    s16 h;
    char pad3[6];
};
extern struct State86 *gMovieAudioState __asm__("D_8009F4D8");
extern void func_80098FE8(struct Setup86 *);
extern s32 func_800452C8(void);
extern void func_80099368(struct Msg86 *);

/**
 * @brief Sends a positioned playback message to the sound engine.
 *
 * First a setup block of type 3 with both scale fields 0x3FFF is handed to
 * func_80098FE8. Then a message of kind 0xFF93 is filled: the object pointer,
 * a panning value that is a random 0..127 only for the two tracked objects
 * (gMovieAudioState->first and ->second), the caller value, and a block of fixed
 * fields (1, 1, 3, 0, 0, 0, 0, 15). func_80099368 consumes the message.
 *
 * @param object Entity the sound is attached to.
 * @param value Payload passed through in the message.
 * @param c Third argument forwarded to the message field at 0x10.
 *
 * @par Globals
 * gMovieAudioState - state block with the two tracked objects.
 *
 * @par Control flow
 * Straight-line fill of two stack blocks.
 *
 * @par Calls
 * func_80098FE8 - setup. func_80099368 - message dispatch.
 * func_800452C8 - random source for the panning values.
 *
 * @par Side effects
 * Emits one engine message; no globals written.
 *
 * @par Callers
 * Confirmed: func_80087824 (sound initialisation).
 */
void func_80086C74(void *object, s32 value, s32 c)
{
    struct Msg86 msg;
    struct Setup86 setup;

    setup.type = 3;
    setup.a = 0x3FFF;
    setup.b = 0x3FFF;
    func_80098FE8(&setup);
    msg.object = object;
    msg.kind = 0xFF93;
    msg.a = object == gMovieAudioState->first ? func_800452C8() * 127 : 0;
    msg.b = object == gMovieAudioState->second ? func_800452C8() * 127 : 0;
    msg.c = c;
    msg.value = value;
    msg.one = 1;
    msg.one2 = 1;
    msg.three = 3;
    msg.d = 0;
    msg.e = 0;
    msg.f = 0;
    msg.g = 0;
    msg.h = 15;
    func_80099368(&msg);
}

