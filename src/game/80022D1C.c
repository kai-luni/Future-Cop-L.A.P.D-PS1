// Categories: Game Engine > State Flags
typedef unsigned char u8;
extern int gMode1IndexBase __asm__("D_800A09D4");

void gMarkAudioStreamAdpcmBlock(u8 *data, int mode, int offset) __asm__("func_80022D1C");

/**
 * @brief Sets the ADPCM block flag byte selected by the audio stream mode.
 *
 * @param data Buffer to modify.
 * @param mode Selects the update; other values leave the buffer unchanged.
 * @param offset Offset used by mode three, before subtracting 15.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Mode 0 ORs 4 at buffer +1. Mode 1 ORs 1 at gMode1IndexBase minus 15. Mode 3 stores 1 at the
 * supplied offset minus 15. Other modes perform no write.
 *
 * @par Globals
 * Reads gMode1IndexBase in mode 1.
 *
 * @par Callers
 * Direct calls from gSubmitAudioStreamTransfer (func_80022D84).
 *
 * @par Role and path
 * The stream request builder prepares the 16-byte ADPCM block flags before submitting the
 * channel buffer.
 *
 * @par Unknown
 * This function does not validate the caller's buffer extent.
 */
void gMarkAudioStreamAdpcmBlock(u8 *data, int mode, int offset)
{
    register int index __asm__("$2");
    register u8 *ptr __asm__("$3");
    if (mode == 1) goto one;
    if (mode == 0) goto zero;
    if (mode == 3) goto three;
    goto done;
zero:
    data[1] |= 4;
    goto done;
one:
    index = gMode1IndexBase - 15;
    __asm__("" : "=r"(index) : "0"(index));
    ptr = data + index;
    __asm__("" : "=r"(ptr) : "0"(ptr));
    *ptr |= 1;
    goto done;
three:
    data[offset - 15] = 1;
done:
    return;
}
