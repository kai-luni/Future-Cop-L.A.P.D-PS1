// Categories: Platform > Memory Card | Resources > Save Data
struct GlobalState {
    unsigned char pad[0x900];
    int value;
};

extern struct GlobalState *gOperationState __asm__("D_8009AB08");

/**
 * @brief Returns the memory-card operation status/result code at D_8009AB08 + 0x900.
 *
 * Dereferences D_8009AB08 and returns field +0x900 (value), which card callbacks and state handlers update with the transfer outcome (-1 pending, negative error codes such as -2/-3, or >= 0 completion).
 *
 * @param void No parameters.
 * @return 32-bit status/result word D_8009AB08->value (+0x900).
 *
 * @par Globals
 * D_8009AB08 - pointer to memory-card state block; reads word at +0x900.
 *
 * @par Control flow
 * Single pointer dereference and load; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80020A9C (0x80020AAC, 0x80020AD0), func_8007FB04 (0x8007FB18), func_8008204C (0x80082074).
 */
int func_800207B4(void)
{
    return gOperationState->value;
}
