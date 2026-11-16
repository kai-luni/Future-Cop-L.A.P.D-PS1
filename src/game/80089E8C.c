// Categories: Platform > State Query
typedef unsigned char u8;
typedef unsigned int u32;
struct State {
    u8 pad0[0x10];
    struct State *next;
    u8 pad14[0x1c];
    u8 *data;
    u32 flags;
    u8 busy;
    u8 pad39[0x10];
    u8 mode;
};
extern struct State *(*D_8009C73C)(void);

/**
 *  @brief Returns a state mode with conditional remapping for active conditions.
 *
 *  The state comes from a registered provider and is checked for upper flag bits,
 *  a busy non-self-linked state, or a nonzero first data byte.
 *  @return Mode 1 for modes 2 or 3, or mode 4 for mode 6, when a condition holds;
 *  otherwise the original mode.
 *  @par Globals
 *  D_8009C73C is the function pointer used to obtain the state.
 *  @par Control flow
 *  Tests the three conditions with short-circuit evaluation, then switches on mode.
 *  @par Calls
 *  Invokes the function pointer stored in D_8009C73C.
 *  @par Side effects
 *  None directly; the provider may have side effects.
 *  @par Callers
 *  Confirmed: func_8001EC94.
 */
int func_80089E8C(void)
{
    struct State *s = D_8009C73C();
    if ((s->flags & 0xffff0000) || (s != s->next && s->busy) || s->data[0]) {
        switch (s->mode) {
        case 2: return 1;
        case 3: return 1;
        case 6: return 4;
        }
    }
    return s->mode;
}
