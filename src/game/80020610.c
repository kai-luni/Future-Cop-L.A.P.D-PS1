// Categories: Utilities > Constants | Resources > Movie Playback
/**
 * @brief Returns the size of the movie decoder context.
 *
 * @return 0x1BB8, always - the byte size that func_8007F9CC() allocates and
 *         that func_80020618() and func_800206D0() work on.
 *
 * @par Callers
 * func_8007F9CC (allocation size) and func_80020618 (clearing the block).
 */
int func_80020610(void)
{
    return 0x1BB8;
}
