// Categories: Platform > Memory Card | Resources > Save Data
struct GlobalState {
    unsigned char pad[0x900];
    int value;
};

extern struct GlobalState *gOperationState __asm__("D_8009AB08");

int gReadCardOperationResult(void) __asm__("func_800207B4");

/**
 * @brief Returns the status word for the current memory-card operation.
 * @return The word at +0x900 of the card manager.
 *
 * @par Confirmed mechanics
 * Loads gOperationState through its absolute address, then loads its word at
 * +0x900 into $v0.
 *
 * @par Globals
 * Reads gOperationState.
 *
 * @par Callers
 * Direct calls in the retail listing are from gIsCardResultUsable,
 * func_8007FB04, gQueueMemoryCardFormat.
 *
 * @par Role and path
 * The memory-card UI and lookup path use this shared status accessor.
 *
 * @par Unknown
 * Individual negative values are interpreted by its callers.
 */
int gReadCardOperationResult(void)
{
    return gOperationState->value;
}
