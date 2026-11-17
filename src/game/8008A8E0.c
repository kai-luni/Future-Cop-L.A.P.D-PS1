// Categories: Platform > Callback Dispatch
#include "game/types.h"
extern s32 gPadDispatchIndex __asm__("D_8009C76C");
extern  s32 (*gPadHandlerTable[])(void) __asm__("D_8009C7AC");
extern void (*D_8009C728)(s32);
extern void func_8008C890(u32);
extern s32 func_8008ADB4(void);

/**
 *  @brief Dispatches the next indexed callback and handles its result.
 *
 *  The index at address 0x8009C76C is incremented before the callback runs.
 *  Successful dispatch can trigger a delay and check, then adjust the index.
 *  @par Globals
 *  D_8009C76C is the dispatch index and controls post-dispatch processing.
 *  D_8009C7AC supplies callbacks; D_8009C728 supplies the error callback.
 *  @par Control flow
 *  Negative results go directly to the error callback; nonnegative results
 *  conditionally run the delay and check, then decrement the index if it is at least 5.
 *  @par Calls
 *  Calls the indexed D_8009C7AC function; func_8008C890 delays with argument 60.
 *  func_8008ADB4 supplies the check; D_8009C728 receives errors or -3 on check failure.
 *  @par Side effects
 *  Updates the dispatch index and invokes callbacks and conditional helper calls.
 *  @par Callers
 *  Confirmed: func_8008A43C.
 */
void func_8008A8E0(void)
{
    s32 result;
    register s32 *index asm("$5") = (s32 *)0x800A0000;
    register s32 n asm("$3");
    register s32 (*fn)(void) asm("$2");
    asm("" : "=r"(index) : "0"(index));
    index = (s32 *)((char *)index - 0x3894);
    asm volatile("" : "=r"(index) : "0"(index));
    n = *index;
    fn = gPadHandlerTable[n];
    *index = n + 1;
    result = fn();
    if (result >= 0) {
        if (gPadDispatchIndex != 0) {
            func_8008C890(60);
            if (!func_8008ADB4()) {
                D_8009C728(-3);
            }
        }
        if (gPadDispatchIndex >= 5) {
            register s32 n2 asm("$2") = gPadDispatchIndex - 1;
            register s32 *p asm("$1");
            asm volatile("" : "=r"(n2) : "0"(n2));
            p = (s32 *)0x800A0000;
            asm volatile("" : "=r"(p) : "0"(p));
            *(s32 *)((char *)p - 0x3894) = n2;
        }
    } else {
        D_8009C728(result);
    }
}
