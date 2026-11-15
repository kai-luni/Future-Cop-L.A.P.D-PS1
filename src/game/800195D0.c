// Categories: Resources > Memory Pools
struct LinkState {
    unsigned char pad_00[0xC];
    int count;
    int *link;
};

/**
 * @brief Pushes a node back onto a fixed-size pool's free list.
 *
 * @param state Pool descriptor: count at +0x0C, free-list head at +0x10.
 * @param link  Node to free; its first word becomes the new list head.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * *link = state->link (the node points at the current head), then the node
 * becomes the head and the count is incremented - a plain LIFO free list.
 *
 * @par Callers
 * func_800181D4, whenever a freed block lies inside one of the three pools;
 * the counterpart of func_800195A4.
 */
void func_800195D0(struct LinkState *state, int *link)
{
    *link = (int)state->link;
    state->link = link;
    state->count++;
}
