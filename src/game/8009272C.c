// Categories: Utilities > Record Manipulation
typedef unsigned char u8;
typedef unsigned int u32;

/**
 *  @brief Combines two record byte values when their sum fits the limit.
 *
 *  Adds the fourth bytes of two records plus one; on success, stores
 *  the sum in the first record and clears the first word of the second.
 *  @param a0 First record, whose fourth byte receives the sum on success.
 *  @param a1 Second record, whose first 32-bit word is cleared on success.
 *  @return 0 on success, or -1 if the sum is at least 0x11.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Rejects an excessive sum before performing either write.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  On success, writes a0[3] and clears the word at a1.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_8009272C(u8 *a0, u8 *a1)
{
    int v;

    v = a0[3] + a1[3] + 1;
    if (v >= 0x11) {
        return -1;
    }
    a0[3] = v;
    *(u32 *)a1 = 0;
    return 0;
}
