struct State {
    char pad0[3];
    unsigned char a;
    char pad1[3];
    unsigned char b;
    char pad2[20];
    unsigned int c;
};

void func_800925D4(struct State *state)
{
    state->a = 7;
    state->b = 0x58;
    state->c = 0x55555555;
}
