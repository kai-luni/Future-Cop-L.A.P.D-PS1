// Categories: Game Engine > World Reset

void gStubGteProjectionTeardown(void) __asm__("func_80019D88");

/**
 * @brief Returns immediately from the projection teardown slot in the mission
 * reset path.
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
 * The mission teardown calls this entry opposite the stage that initializes
 * projection state.
 *
 * @par Unknown
 * Any cleanup once intended for this slot is open.
 */
void gStubGteProjectionTeardown(void)
{
}
