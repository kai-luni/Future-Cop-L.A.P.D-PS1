// Categories: Platform > CD-ROM | Resources > File Access
extern unsigned char gCdTargetLocation[] __asm__("D_8009C8C0");

/**
 * @brief Returns a pointer to the 4-byte CD-ROM target location buffer D_8009C8C0.
 *
 * Exposes D_8009C8C0 (the active CdlLOC minute/second/sector/track buffer updated by func_8008F244) for CD Setloc commands and LBA conversion in func_80090C18. Category Platform > CD-ROM | Resources > File Access follows the CD-ROM driver state buffer and caller usage.
 *
 * @param void No parameters.
 * @return Pointer to D_8009C8C0.
 *
 * @par Globals
 * D_8009C8C0 - 4-byte CD-ROM CdlLOC position buffer whose address is returned.
 *
 * @par Control flow
 * Straight-line address load and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80090C18 (0x80090CEC and 0x80090D70 in asm/us/main.s) / Likely: none / Unknown: none.
 */
unsigned char *func_8008E040(void)
{
    return gCdTargetLocation;
}
