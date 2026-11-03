#include "game/types.h"

typedef struct {
    u8 pad00[0x11E];
    s16 unk11E;
    u8 pad120[0x15F - 0x120];
    u8 unk15F;
    u8 pad160[0x163 - 0x160];
    u8 unk163;
} Object8002C3F0;

typedef struct {
    u8 pad00[8];
    void *unk08;
} Data8002C3F0;

extern void *jtbl_8001104C[];

extern void func_800379D4(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern void func_8002BFB4(Object8002C3F0 *object, void *data);
extern void func_8002AF94(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern void func_80037B28(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern s32 func_8002AD58(s32 message, Object8002C3F0 *object, Data8002C3F0 *data);
extern void func_8002C174(Object8002C3F0 *object);
extern void func_8002B784(Object8002C3F0 *object, Data8002C3F0 *data, s32 arg3);
extern s32 func_8002ACB4(Object8002C3F0 *object, s32 value);

void func_8002C3B0(s32 message, Object8002C3F0 *object, Data8002C3F0 *data)
{
    func_800379D4(message, object, data);
    func_8002BFB4(object, data);
}
