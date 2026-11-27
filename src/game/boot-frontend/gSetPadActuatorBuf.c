// Categories: Input > DualShock | Platform > Controller
extern void *(*gPadLookupPortFn)(int) __asm__("D_8009C73C");
extern void gBindActuatorBuffer(void *, int, int) __asm__("func_8008AE70");
void gSetPadActuatorBuf(int arg0, int arg1, int arg2) __asm__("func_8008A240");


/**
 * @brief Binds a rumble actuator data buffer to a controller port's CommandState (PadSetAct).
 *
 * Resolves the port's CommandState pointer through the indirect lookup callback D_8009C73C(arg0)
 * and forwards it with the caller's actuator buffer pointer (arg1) and byte length (arg2) to func_8008AE70.
 *
 * @param arg0 Controller port/slot index passed to D_8009C73C.
 * @param arg1 Pointer (cast to int) to the caller's actuator motor output buffer.
 * @param arg2 Byte count / actuator buffer type passed to func_8008AE70.
 *
 * @par Globals
 * D_8009C73C - indirect port lookup function pointer returning the CommandState for port arg0.
 *
 * @par Control flow
 * Straight-line indirect lookup via D_8009C73C(arg0) followed by tail/nested call to func_8008AE70.
 *
 * @par Calls
 * D_8009C73C (indirect) - resolves port CommandState; func_8008AE70 - stores buffer pointer (+0x28) and count (+0x34).
 *
 * @par Side effects
 * Updates the target port's CommandState actuator buffer pointer and length via func_8008AE70.
 *
 * @par Callers
 * Confirmed: func_8001EC94 (0x8001EDA4, binds s->values of length 2 during DualShock actuator setup) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Resolves the port CommandState for arg0 via D_8009C73C(arg0) and passes it
 * with buffer pointer arg1 and length arg2 to func_8008AE70. Called by
 * func_8001EC94 during DualShock rumble actuator setup.
 */
void gSetPadActuatorBuf(int arg0, int arg1, int arg2)
{
    gBindActuatorBuffer(gPadLookupPortFn(arg0), arg1, arg2);
}
