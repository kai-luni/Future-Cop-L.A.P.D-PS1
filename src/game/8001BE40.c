// Categories: Utilities > No-op

void gNoOpAt8001BE40(void) __asm__("func_8001BE40");

/**
 * @brief Returns immediately from the empty entry at 0x8001BE40.
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
 * This is the first of five consecutive empty entries between render helpers
 * and GPU setup.
 *
 * @par Unknown
 * No caller or specific interface for this entry is established.
 */
void gNoOpAt8001BE40(void)
{
}
