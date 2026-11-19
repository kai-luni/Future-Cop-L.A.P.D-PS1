unsigned int gPackDrawOffset(unsigned int x, unsigned int y) __asm__("func_800940A8");

// Categories: Graphics > GPU Primitives > Texture
/**
 * @brief Builds the GPU texture-window/UV coordinate word (opcode 0xE5).
 *
 * @param x, y Texture coordinates (masked to 11 bits each).
 * @return 0xE5000000 | (y << 11) | x.
 *
 * @par Confirmed
 * Pure packing: y occupies bits 11..21, x bits 0..10, the 0xE5 opcode
 * byte identifies the PSX GPU texcoord command.
 *
 * @par Likely role
 * The UV word emitter of the sprite packet builders (func_80093AF0,
 * func_80093A0C, func_80093CE8): one of the fixed GPU command packers
 * alongside the draw-mode and tpage words.
 *
 * @par Callers
 * func_80093A0C, func_80093AF0, func_80093CE8.
 *
 * @par Environment path
 * Packs the GP0 E5 drawing-offset command, the x,y added to every vertex of
 * later primitives (not a texture window, which is E2). Used by func_80093A0C
 * (2-word offset packet), func_80093AF0 and func_80093CE8 when building
 * draw-environment lists.
 */
unsigned int gPackDrawOffset(unsigned int x, unsigned int y)
{
    return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
}
