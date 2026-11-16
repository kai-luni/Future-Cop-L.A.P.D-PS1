// Categories: Gameplay > Entity Update | AI > Command Processing
typedef unsigned short u16;

struct ValuePair {
    unsigned char pad[0x10C];
    u16 source;
    u16 destination;
};

/**
 * @brief Copies the u16 reload field at +0x10C into the countdown field at +0x10E.
 *
 * Loads the unsigned halfword at +0x10C (ValuePair.source) and stores it into +0x10E (ValuePair.destination) on a class-26 actor block.
 *
 * @param value Pointer to actor ValuePair block (+0x10C source, +0x10E destination).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line halfword copy (lhu 0x10C($a0); jr $ra; sh 0x10E($a0)).
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes *(u16 *)((u8 *)value + 0x10E).
 *
 * @par Callers
 * Confirmed: func_8002B784 (0x8002BB74, class-26 per-frame update) / Likely: none / Unknown: none.
 */
void func_8002B1D0(struct ValuePair *value)
{
    value->destination = value->source;
}
