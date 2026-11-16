// Categories: Platform > Hardware Control
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern s32 D_8009CC8C[];
extern volatile short *D_8009DD18;
extern void func_8008D7B0(s32);
extern void func_8008CA20(s32, s32);

/**
 *  @brief Updates an indexed control value and its hardware mask when enabled.
 *
 *  Reads the previous value from the control table and, if it changes,
 *  synchronizes the table, shadow mask, and hardware register when active.
 *  @param index Control-table index and corresponding mask bit.
 *  @param value New control value; zero clears the corresponding bit.
 *  @return Previous table value, even if no update was performed.
 *  @par Globals
 *  D_8009CC8C supplies the table and adjacent state; D_8009DD18 points to the hardware register.
 *  @par Control flow
 *  Returns early for an unchanged value; active state gates updates and index-specific calls.
 *  @par Calls
 *  func_8008D7B0 handles index 0; func_8008CA20 handles indices 0 and 4 through 6.
 *  @par Side effects
 *  May update the table, shadow mask, hardware register, and callee-managed state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_800918BC(s32 index, s32 value)
{
    register s32 *base __asm__("$5") = D_8009CC8C;
    s32 old = base[index];
    register u32 bits __asm__("$19");
    register u32 sample __asm__("$3");
    register char *state __asm__("$6");
    volatile short *hw;
    register u32 setmask __asm__("$3");
    register u32 clearmask __asm__("$2");
    if (value == old) return old;
    state = (char *)base - 4;
    if (*(u16 *)((char *)base - 4)) {
        hw = D_8009DD18;
        sample = (u16)*hw;
        *hw = 0;
        __asm__("" : "+r"(sample) : "m"(*hw));
        bits = (u16)sample;
        if (value) {
            setmask = 1 << index;
            base[index] = value;
            bits |= setmask;
            *(u16 *)(state + 0x30) |= setmask;
        } else {
            clearmask = ~(1 << index);
            base[index] = 0;
            bits &= clearmask;
            *(volatile u16 *)((char *)base + 0x2c) &= clearmask;
        }
        if (index == 0) {
            s32 disabled = !value;
            func_8008D7B0(disabled);
            func_8008CA20(3, disabled);
        }
        if (index == 4) func_8008CA20(0, !value);
        if (index == 5) func_8008CA20(1, !value);
        if (index == 6) func_8008CA20(2, !value);
        *D_8009DD18 = bits;
    }
    return old;
}
