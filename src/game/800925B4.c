struct State {
    char pad0[3];
    unsigned char field3;
    char pad4[3];
    unsigned char field7;
    char pad8[12];
    unsigned int field14;
};

void func_800925B4(struct State *state)
{
    state->field3 = 5;
    state->field7 = 0x48;
    state->field14 = 0x55555555;
}
