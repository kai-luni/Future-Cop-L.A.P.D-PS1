// Categories: Platform > Memory Card | Resources > Save Data
struct GlobalState {
    unsigned char pad[0x9A4];
    int value;
};

extern struct GlobalState *gOperationState __asm__("D_8009AB08");
int gGetMcPendingFlag(void) __asm__("func_8001F37C");


/**
 * @brief Returns the memory-card manager pending/busy status word at D_8009AB08 + 0x9A4.
 *
 * Reads D_8009AB08->value (+0x9A4), which memory-card queue helpers set while an operation is active and interrupt handlers clear to 0 once finished.
 *
 * @param void No parameters.
 * @return Current 32-bit busy/pending word D_8009AB08->value (+0x9A4).
 *
 * @par Globals
 * D_8009AB08 - pointer to memory-card state block; reads word at +0x9A4.
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
 * Confirmed: func_80020D3C (0x80020DBC), func_80020FDC (0x80021094), func_8007FB04 (0x8007FBEC).
 *
 * @par Role and path
 * Returns the 32-bit pending/status word at offset +0x9A4 of gOperationState
 * (D_8009AB08). Reads D_8009AB08 and makes no calls; called by func_80020D3C,
 * func_80020FDC, and func_8007FB04.
 */
int gGetMcPendingFlag(void)
{
    return gOperationState->value;
}
