#include "game/types.h"

struct Command {
    s8 op;
    s8 arg;
};

struct Blah {
    u8 unk00[0x0A];
    struct Command cur;
    struct Command next;
};

void func_8002A5B4(u8 *script, s32 index)
{
    register u8 *base __asm__("$8");
    register struct Blah *p __asm__("$6");
    s32 i;
    register s32 off __asm__("$2");

    base = script;
    i = index + 1;
    if (i < 4) {
        off = i * 2;
        p = (struct Blah *)(off + (u32)base);
        do {
            p->cur = p->next;
            i++;
            p = (struct Blah *)((u8 *)p + 2);
        } while (i < 4);
    }
    base[0x12] = 0;
}
