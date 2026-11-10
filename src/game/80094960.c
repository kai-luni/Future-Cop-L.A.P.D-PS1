extern unsigned int *D_8009DEC8;
extern unsigned int *D_8009DEC4;

int func_80094960(unsigned int *data, unsigned int count)
{
    unsigned int remaining = count - 1;

    *D_8009DEC8 = 0x04000000;
    if (count != 0) {
        do {
            *D_8009DEC4 = *data++;
        } while (remaining-- != 0);
    }
    return 0;
}
