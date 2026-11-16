// Categories: Resources > Lookup
typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_8009C94C[];
extern u8 D_8001233C[];

/**
 *  @brief Looks up a pointer using the low byte of an index.
 *
 *  Low-byte values below seven select entries from D_8009C94C;
 *  all other low-byte values select the fallback D_8001233C.
 *  @param index Index whose low eight bits are used.
 *  @return The selected table pointer or the fallback pointer.
 *  @par Globals
 *  D_8009C94C supplies table entries; D_8001233C is the fallback address.
 *  @par Control flow
 *  Masks the index, then branches on whether it is at least seven.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u8 *func_8008E120(u32 index)
{
    u32 i = index & 0xFF;

    if (i >= 7) {
        return D_8001233C;
    }
    return D_8009C94C[i];
}
