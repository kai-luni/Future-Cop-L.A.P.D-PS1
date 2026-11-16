// Categories: Game Engine > Callback Dispatch
struct CallbackTable {
    void *reserved[14];
    int (*callback)(void);
};

extern struct CallbackTable *D_8009DDE8;

/**
 *  @brief Returns the sign bit of a callback's result.
 *
 *  Invokes the callback at offset 0x38 in the table referenced by D_8009DDE8.
 *  The callback result is converted to unsigned before its top bit is extracted.
 *  @return 1 if the callback result has its sign bit set, otherwise 0.
 *  @par Globals
 *  D_8009DDE8 supplies the callback table pointer.
 *  @par Control flow
 *  Makes one indirect call and shifts its result right by 31 bits.
 *  @par Calls
 *  The table callback is invoked with no arguments.
 *  @par Side effects
 *  Any side effects are those of the invoked callback.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
unsigned int func_80093924(void)
{
    return (unsigned int)D_8009DDE8->callback() >> 31;
}
