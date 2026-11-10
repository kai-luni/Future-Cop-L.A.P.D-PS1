struct State {
    unsigned char pad0[3];
    unsigned char active;
    unsigned char pad4[3];
    unsigned char current;
    unsigned char pad8[16];
    unsigned int value;
};

void func_800925F4(struct State *state)
{
    state->active = 6;
    state->current = 0x4C;
    state->value = 0x55555555;
}
