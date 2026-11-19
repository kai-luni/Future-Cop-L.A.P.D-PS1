// Categories: Resources > Memory Pools
struct LinkState {
    unsigned char pad_00[0xC];
    int count;
    int *link;
};

void gReturnFixedPoolSlot(struct LinkState *state, int *link) __asm__("func_800195D0");

/**
 * @brief Pushes a slot onto the free list of a fixed-size pool.
 *
 * @param state Pool descriptor.
 * @param link Slot to return.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores the old head into the slot's first word, installs the slot at pool
 * offset +0x10, and increments +0x0C in the return delay slot.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_800181D4, func_800295E8,
 * func_800497A8, func_800498F0, func_80083834, func_800841C4.
 *
 * @par Role and path
 * The heap and gameplay pool paths call this to make a released slot available
 * again.
 *
 * @par Unknown
 * The caller is responsible for supplying a slot owned by the pool.
 */
void gReturnFixedPoolSlot(struct LinkState *state, int *link)
{
    *link = (int)state->link;
    state->link = link;
    state->count++;
}
