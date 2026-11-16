// Categories: Graphics > Lighting | Graphics > Camera
extern unsigned char *gCamera __asm__("D_8009F9E4");
extern unsigned char D_8009A228[];
extern unsigned char D_800A02E0[];
extern void func_8001A81C(void *, void *, void *);

/**
 * @brief Rotates the fixed world vector D_8009A228 into the camera frame.
 *
 * D_8009F9E4 holds the active camera and its 3x3 orientation matrix sits at +0x28; func_8001A81C multiplies it with the 12-byte constant vector at D_8009A228 in 16.16 fixed point and writes the result to D_800A02E0, the light-direction source that func_80013F08 and func_8001428C read.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F9E4 - camera pointer, read only. D_8009A228 - constant input vector, read only. D_800A02E0 - 12-byte output vector, written.
 *
 * @par Control flow
 * Straight line: matrix pointer biased by 0x28, one call, return.
 *
 * @par Calls
 * func_8001A81C - matrix times vector in 16.16 fixed point.
 *
 * @par Side effects
 * D_800A02E0 is overwritten with the rotated vector. Because the camera matrix is rebuilt every frame this must run once per frame before the scene pass.
 *
 * @par Callers
 * Confirmed: func_80027510 (0x800278E0, 0x80027AEC) in the per-player frame block, right before the map-screen row update.
 * Unknown: one call at 0x800273C0 from a routine that is not in the matching set.
 */
void func_80013C80(void)
{
    func_8001A81C(gCamera + 0x28, D_8009A228, D_800A02E0);
}
