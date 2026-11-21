// Categories: Gameplay > Object State | Game Engine > Entity Lifecycle
typedef unsigned short u16;

struct FlagValue {
    unsigned char pad[0xE];
    u16 flags;
};
void gSetObjectFlag1000(struct FlagValue *value) __asm__("func_80073BA8");


/**
 * @brief Sets flag bit 0x1000 in an object's flag word.
 *
 * @param value Record with the u16 flag word at +0xE.
 * @return Nothing.
 *
 * @par Confirmed
 * Single read-modify-write: value->flags |= 0x1000. No other effect.
 *
 * @par Likely role
 * A one-bit state marker of the effect handler func_80073BBC (likely
 * "already spawned" or "active"), set through a helper so the call site
 * stays a plain function pointer.
 *
 * @par Unknown
 * Which state bit 0x1000 encodes.
 *
 * @par Callers
 * func_80073BBC.
 */
void gSetObjectFlag1000(struct FlagValue *value)
{
    value->flags |= 0x1000;
}
