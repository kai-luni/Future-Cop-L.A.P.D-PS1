// Categories: Platform > Memory Card | Resources > File Access
extern int func_800207B4(int);

/**
 * @brief Turns a memory-card poll result into a completion flag.
 *
 * -2 and -1 both count as success; -3 triggers one more poll, after which -2
 * still succeeds. Any other value is normalised through an unsigned bit trick
 * ((~value) >> 31), i.e. "value was negative". The wrapper spares callers the
 * raw error codes.
 * @param arg0 Slot/handle passed to the poll function.
 * @return 1 on completion, 0 otherwise.
 * @par Globals
 * None.
 * @par Control flow
 * Two nested code tests.
 * @par Calls
 * func_800207B4 - the poll.
 * @par Side effects
 * May advance the card state machine.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
int func_80020A9C(int arg0)
{
    register int value __asm__("$4");
    register int result __asm__("$2");

    value = func_800207B4(arg0);
    if (value == -2)
        return 1;
    if (value == -3) {
        value = func_800207B4(arg0);
        if (value == -2)
            return 1;
        result = 1;
        __asm__("" : "+r"(result));
        if (value == -1)
            return result;
        return (unsigned int)~value >> 31;
    }
    if (value == -1)
        return 1;
    return (unsigned int)~value >> 31;
}
