// Categories: Input > Controller > Rumble | Input > Controller
extern void func_8001EBA8(int index);

/**
 * @brief Stops the vibration motors on controller ports 0 and 1.
 *
 * Loops the two ports and calls func_8001EBA8, which clamps its port to 0..1 and clears both actuators: the small motor byte and timer through func_8001E9AC, the large motor level and timer through func_8001EA5C.
 *
 * @return Nothing.
 *
 * @par Globals
 * None directly; the actuator records D_800A6FF8 are cleared inside func_8001E9AC and func_8001EA5C.
 *
 * @par Control flow
 * for loop over index 0 and 1, two iterations, no early exit.
 *
 * @par Calls
 * func_8001EBA8 - stops both motors of one port.
 *
 * @par Side effects
 * Motor bytes in D_800A6FF8 are zeroed and both motor timers reset, so the pads stop vibrating immediately. func_80027510 calls this when the pause path closes, func_8001EC04 when rumble is switched off in the options.
 *
 * @par Callers
 * Confirmed: func_80025B7C (0x80025B98) and func_80027510 (0x80027CD8), the pause path of the frame driver.
 * Unknown: one call at 0x800274CC from a routine that is not in the matching set.
 */
void func_8001EB6C(void)
{
    int index;

    for (index = 0; index < 2; index++) {
        func_8001EBA8(index);
    }
}
