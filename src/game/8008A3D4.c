typedef struct {
    unsigned int first;
    unsigned int second;
} Flags;

extern Flags *D_8009C788;
extern void (*D_8009C750)(void);

int func_8008A3D4(void)
{
    if (!(D_8009C788->second & 1))
        return 0;
    if (!(D_8009C788->first & 1))
        return 0;
    if (D_8009C750 != 0)
        D_8009C750();
    return 1;
}
