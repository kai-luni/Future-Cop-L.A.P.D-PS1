typedef struct State {
    char pad[0x7B20];
    int active;
    volatile int value;
    volatile int count;
} State;

extern State *D_8009F4D8;

void func_80086D74(void)
{
    State *state = D_8009F4D8;
    if (state->active != 0) {
        state->value += 0x100;
        state->count++;
    }
}
