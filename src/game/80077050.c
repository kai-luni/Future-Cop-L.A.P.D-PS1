// Categories: Resources > Memory Pools
extern void func_80017C68(int, int, int, int, int, int);

/**
 * @brief (Re)installs the three fixed-size memory pools with an 8-byte stride
 *        and zero entries.
 *
 * func_80017C68(8, 0, 8, 0, 8, 0) stores the argument pairs as
 * (size, count) for the three pools: D_8009F9D0 from (a5, a4), D_8009FA4C from
 * (a1, a0) and D_8009FAAC from (a3, a2) - all 8 bytes wide with a count of 0,
 * and rewrites the four words of the range table so that it spans nothing.
 *
 * The pools are unusable in this state (the allocator requires a positive
 * count), so allocation mode 3 falls through to mode 1; the call is therefore
 * "reset the pool layer", not "provide memory".
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80076A2C, before the render arena is allocated and once per frame
 * before the blits; func_80077088() undoes it.
 */
void func_80077050(void)
{
    func_80017C68(8, 0, 8, 0, 8, 0);
}
