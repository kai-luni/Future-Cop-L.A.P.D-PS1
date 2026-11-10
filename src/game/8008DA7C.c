typedef struct {
    unsigned int previous;
    unsigned int current;
} InputState;

extern InputState *D_8009C808;

int func_8008DA7C(void)
{
    if (D_8009C808->current & 1) {
        register unsigned int previous __asm__("$2") = D_8009C808->previous & 1;
        __asm__ volatile ("" : "+r" (previous));
        if (previous) {
            return 1;
        }
    }
    return 0;
}
