// Categories: Input > State
typedef struct {
    unsigned int previous;
    unsigned int current;
} InputState;

extern InputState *D_8009C808;

/**
 *  @brief Tests whether input bit zero is set in both sampled states.
 *
 *  Reads the current and previous input words from the shared input state
 *  to identify a bit that remains set across both samples.
 *  @return 1 if bit zero is set in both words, otherwise 0.
 *  @par Globals
 *  D_8009C808 points to the input state containing both words.
 *  @par Control flow
 *  Checks the previous bit only when the current bit is set.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_8008DA7C(void)
{
    if (D_8009C808->current & 1) {
        register unsigned int previous __asm__("$2") = D_8009C808->previous & 1;
        __asm__ volatile ("" : "+r" (previous));
        if (previous) {
            return 1;
        }
    }
    return 0;
}
