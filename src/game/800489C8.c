// Categories: Graphics > Text > Rendering
struct LookupState {
    unsigned char pad_00[0x10];
    unsigned char *values;
};
unsigned char *gLookupFontGlyph(struct LookupState *state, unsigned int offset) __asm__("func_800489C8");


/**
 * @brief Looks up the glyph record of a character in a font.
 *
 * @param state Font lookup state; values points at the glyph records.
 * @param offset Character offset; the index byte is read at
 *               state + offset + 0x1C.
 * @return Pointer to the 11-byte glyph record (values + index * 11).
 *
 * @par Confirmed
 * Single byte load and scaled pointer add. The 11-byte stride matches the
 * glyph table built by func_80048A14.
 *
 * @par Callers
 * func_80048F98, func_800492A0, func_80049340.
 */
unsigned char *gLookupFontGlyph(struct LookupState *state, unsigned int offset)
{
    unsigned char index = *((unsigned char *)state + offset + 0x1C);
    return state->values + index * 11;
}
