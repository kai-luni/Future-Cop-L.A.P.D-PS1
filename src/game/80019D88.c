// Categories: Game Engine > World Reset

void gStubMissionReset(void) __asm__("func_80019D88");

/**
 * @brief Returns immediately from an empty entry in the mission reset
 * sequence.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * The two instructions are JR $ra and a NOP in its delay slot.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gResetMissionSubsystems.
 *
 * @par Role and path
 * The mission teardown calls this stub once during subsystem reset.
 *
 * @par Unknown
 * The specific subsystem once associated with this entry is open.
 *
 * @par Environment path
 * The mission subsystem reset calls this empty entry at 0x80024E08. The
 * immediate return leaves engine and GTE state intact.
 */
void gStubMissionReset(void)
{
}
