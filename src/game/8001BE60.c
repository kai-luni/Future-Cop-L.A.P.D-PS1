// Categories: Game Engine > Unclassified

void gNoOpAt8001BE60(void) __asm__("func_8001BE60");

/**
 * @brief Returns immediately from the empty entry at 0x8001BE60.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Contains JR $ra with a NOP in its delay slot.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * This is the fifth of five consecutive empty entries before GPU setup.
 *
 * @par Unknown
 * No caller or specific interface for this entry is established.
 */
void gNoOpAt8001BE60(void)
{
}
