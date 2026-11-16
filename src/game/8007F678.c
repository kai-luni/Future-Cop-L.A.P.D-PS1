// Categories: Game Engine > State
extern unsigned char *D_8009F468;

/**
 *  @brief Writes the byte value two through a global pointer.
 *
 *  The function stores 2 at the address held in D_8009F468 and returns
 *  the same value; the byte's meaning is not established by the source.
 *  @return The integer value 2.
 *  @par Globals
 *  D_8009F468 supplies the destination pointer.
 *  @par Control flow
 *  Stores the value and returns it without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the byte pointed to by D_8009F468.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_8007F678(void)
{
    int x = 2;
    *D_8009F468 = x;
    return x;
}
