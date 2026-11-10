typedef struct {
    char pad[10];
    volatile short value;
} State;

extern State *D_8009C804;

int func_8008DA14(void)
{
    volatile int count[3];
    D_8009C804->value = 0;
    count[0] = 10;
    while (--count[0] != -1) {
    }
    return 0;
}
