// Categories: Graphics > Command Transfer
extern unsigned int *D_8009DEC8;
extern unsigned int *D_8009DEC4;

/**
 *  @brief Writes a command followed by a sequence of data words.
 *
 *  Sends 0x04000000 to the command destination, then writes count words
 *  from the supplied array to the data destination.
 *  @param data Source array of unsigned words.
 *  @param count Number of source words to write.
 *  @return Always 0.
 *  @par Globals
 *  D_8009DEC8 points to the command destination; D_8009DEC4 points to data.
 *  @par Control flow
 *  Skips the data loop for zero count; otherwise writes exactly count words.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes to the locations addressed by the two global pointers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_80094960(unsigned int *data, unsigned int count)
{
    unsigned int remaining = count - 1;

    *D_8009DEC8 = 0x04000000;
    if (count != 0) {
        do {
            *D_8009DEC4 = *data++;
        } while (remaining-- != 0);
    }
    return 0;
}
