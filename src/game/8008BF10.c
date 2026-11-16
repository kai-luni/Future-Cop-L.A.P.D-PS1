// Categories: Game Engine > State
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct State {
    u32 a, b, c;
    u8 pad0c[8];
    u32 x, y;
    u8 pad1c[0x2a];
    u8 flag46;
    u8 pad47[2];
    u8 flag49;
    u8 pad4a[0x13];
    u8 entries[6];
    u8 pad63[0x80];
    u8 e3, e4;
    u8 pade5;
    volatile u16 e6;
    u8 pade8;
    u8 e9, ea;
};

/**
 *  @brief Resets selected state fields when the reset flag is set.
 *
 *  Clears counters and flags in the supplied state and marks all six entries
 *  with 0xff; an unset flag leaves the state unchanged.
 *  @param s State containing the reset flag and fields to clear.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Tests flag49, then clears fields and loops over six entries.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the supplied state, including two volatile writes to e6.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void func_8008BF10(struct State *s)
{
    int i;
    int fill;
    u8 *p;
    if (s->flag49) {
        p = s->entries;
        fill = 0xff;
        i = 5;
        s->flag49 = 0;
        s->flag46 = 0;
        s->e6 = 0;
        s->x = 0;
        s->y = 0;
        s->e3 = 0;
        s->e4 = 0;
        s->e6 = 0;
        s->e9 = 0;
        s->ea = 0;
        s->a = 0;
        s->b = 0;
        s->c = 0;
        do {
            *p++ = fill;
        } while (--i >= 0);
    }
}
