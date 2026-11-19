// Categories: Platform > CD-ROM | Resources > Callbacks

void gNoOpCdReadCompletion(void) __asm__("func_8001C60C");

/**
 * @brief Provides an empty completion callback for the CD read path.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Contains JR $ra with a NOP in its delay slot. The address is taken at
 * 0x8001CEDC as gCdlLocToByteOffset plus 0x84.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * The CD scheduler installs this callback when the incoming completion pointer
 * is null.
 *
 * @par Unknown
 * The callback ignores the request's context.
 */
void gNoOpCdReadCompletion(void)
{
}
