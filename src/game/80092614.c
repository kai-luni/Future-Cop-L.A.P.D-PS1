struct State {
    unsigned char padding0[3];
    unsigned char field3;
    unsigned char padding4[3];
    unsigned char field7;
    unsigned char padding8[28];
    unsigned int field24;
};

void func_80092614(struct State *state)
{
    state->field3 = 9;
    state->field7 = 0x5c;
    state->field24 = 0x55555555;
}
